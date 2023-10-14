/*
 * Copyright (c) 2020-present GiarldXin(Gxin)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GX_GMUTEX_H
#define GX_GMUTEX_H

#include "base.h"
#include "gglobal.h"

#include "graii.h"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <shared_mutex>
#include <exception>


GX_NS_BEGIN

using GMutex = std::mutex;

using GRecursiveMutex = std::recursive_mutex;


class GNoLock
{
public:
    explicit GNoLock() = default;

    ~GNoLock() = default;

    void lock() noexcept
    {}

    bool try_lock() noexcept
    { return true; }

    void unlock() noexcept
    {}
};


class GSpinLock
{
public:
    explicit GSpinLock() = default;

    ~GSpinLock() = default;

    GSpinLock(const GSpinLock &b) = delete;

    GSpinLock &operator=(const GSpinLock &b) = delete;

public:
    void lock() noexcept
    {
        while (mFlag.test_and_set()) {}
    }

    void unlock() noexcept
    {
        mFlag.clear();
    }

private:
    std::atomic_flag mFlag{false};
};


class GRWLock
{
public:
    explicit GRWLock() = default;

    ~GRWLock() = default;

    GRWLock(const GRWLock &b) = delete;

    GRWLock &operator=(const GRWLock &b) = delete;

public:
    void readLock()
    {
        mMutex.lock_shared();
    }

    void readUnlock()
    {
        mMutex.unlock_shared();
    }

    void writeLock()
    {
        mMutex.lock();
    }

    void writeUnlock()
    {
        mMutex.unlock();
    }

    GRaii readGuard() noexcept
    {
        return makeRAII(*this, &GRWLock::readUnlock, &GRWLock::readLock);
    }

    GRaii writeGuard() noexcept
    {
        return makeRAII(*this, &GRWLock::writeUnlock, &GRWLock::writeLock);
    }

private:
    std::shared_mutex mMutex;
};


class GSpinRWLock
{
public:
    explicit GSpinRWLock(bool writeFirst = false)
            : mWriteFirst(writeFirst)
    {
    }

    ~GSpinRWLock() = default;

    GSpinRWLock(const GSpinRWLock &b) = delete;

    GSpinRWLock &operator=(const GSpinRWLock &b) = delete;

public:
    void readLock()
    {
        if (std::this_thread::get_id() != this->mWriteTid) {
            int count;
            if (mWriteFirst) {
                do {
                    while ((count = mLockCount) == -1 || mWriteWaitCount > 0) {}
                } while (!mLockCount.compare_exchange_weak(count, count + 1));
            } else {
                do {
                    while ((count = mLockCount) == -1) {}
                } while (!mLockCount.compare_exchange_weak(count, count + 1));
            }
        }
    }

    void readUnlock()
    {
        if (std::this_thread::get_id() != this->mWriteTid) {
            --mLockCount;
        }
    }

    void writeLock()
    {
        if (std::this_thread::get_id() != this->mWriteTid) {
            ++mWriteWaitCount;
            for (int zero = 0; !this->mLockCount.compare_exchange_weak(zero, -1); zero = 0) {}
            --mWriteWaitCount;
            mWriteTid = std::this_thread::get_id();
        }
    }

    void writeUnlock()
    {
        if (std::this_thread::get_id() != this->mWriteTid) {
            throw std::runtime_error("writeLock/Unlock mismatch");
        }
        assert(-1 == mLockCount);
        mWriteTid = NULL_THREAD;
        mLockCount.store(0);
    }

    GRaii readGuard() noexcept
    {
        return makeRAII(*this, &GSpinRWLock::readUnlock, &GSpinRWLock::readLock);
    }

    GRaii writeGuard() noexcept
    {
        return makeRAII(*this, &GSpinRWLock::writeUnlock, &GSpinRWLock::writeLock);
    }

private:
    const std::thread::id NULL_THREAD{};

    const bool mWriteFirst;
    std::thread::id mWriteTid{};
    std::atomic_int mLockCount{0};
    std::atomic_uint mWriteWaitCount{0};
};


template<class MUTEX>
class GLockerGuard
{
public:
    using mutex_type = MUTEX;

    explicit GLockerGuard(MUTEX &mtx)
            : mMutex(mtx)
    {
        mMutex.lock();
    }

    ~GLockerGuard() noexcept
    {
        mMutex.unlock();
    }

    GLockerGuard(const GLockerGuard &) = delete;

    GLockerGuard &operator=(const GLockerGuard &) = delete;

private:
    MUTEX &mMutex;
};


template<typename MUTEX>
using GLocker = std::unique_lock<MUTEX>;

GX_NS_END

#endif //GX_GMUTEX_H
