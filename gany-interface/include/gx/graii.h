/*
 * Copyright (c) 2022 Gxin
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

#ifndef GX_GRAII_H
#define GX_GRAII_H

#include "base.h"
#include "gglobal.h"

#include <functional>
#include <cassert>


GX_NS_BEGIN

template<typename T>
struct no_const
{
    using type = typename std::conditional<std::is_const<T>::value, typename std::remove_const<T>::type, T>::type;
};


class GRaii
{
public:
    using Func = std::function<void()>;

public:
    explicit GRaii(Func release, const Func &execute = [] {}, bool commit = true)
            : mIsCommit(commit),
              mRelease(std::move(release))
    {
        execute();
    }

    GRaii(GRaii &&other) noexcept
            : mIsCommit(other.mIsCommit),
              mRelease(std::move(other.mRelease))
    {
        other.mIsCommit = false;
    }

    GRaii(const GRaii &) = delete;

    GRaii &operator=(const GRaii &) = delete;

    ~GRaii()
    {
        if (mIsCommit && mRelease) {
            mRelease();
        }
    }

public:
    GRaii &commit(bool c = true)
    {
        mIsCommit = c;
        return *this;
    }

private:
    bool mIsCommit = false;
    Func mRelease;
};


template<typename T>
class GRaiiVar
{
public:
    using Self = GRaiiVar<T>;
    using ExecFunc = std::function<T()>;
    using RelFunc = std::function<void(T &)>;

public:
    explicit GRaiiVar(const ExecFunc &execute, RelFunc release, bool commit = true)
            : mIsCommit(commit),
              mResource(execute()),
              mRelease(std::move(release))
    {}

    GRaiiVar(GRaiiVar &&other) noexcept
            : mIsCommit(other.mIsCommit),
              mResource(std::move(other.mResource)),
              mRelease(std::move(other.mRelease))
    {
        other.mIsCommit = false;
    }

    GRaiiVar(const GRaiiVar &) = delete;

    GRaiiVar &operator=(const GRaiiVar &) = delete;

    ~GRaiiVar()
    {
        if (mIsCommit && mRelease) {
            mRelease(mResource);
        }
    }

public:
    Self &commit(bool c = true)
    {
        mIsCommit = c;
        return *this;
    }

    T &get()
    {
        return mResource;
    }

    T &operator*()
    {
        return get();
    }

    template<typename _T=T>
    typename std::enable_if<std::is_pointer<_T>::value, _T>::type operator->() const
    {
        return mResource;
    }

    template<typename _T=T>
    typename std::enable_if<std::is_class<_T>::value, _T *>::type operator->()
    {
        return std::addressof(mResource);
    }

private:
    bool mIsCommit = true;
    T mResource;
    RelFunc mRelease;
};

/**
 * Create RAII.
 *
 * @tparam RES          Resource type
 * @tparam F_REL        Member function address for releasing resources
 * @tparam F_EXEC       Member function address for resource acquisition
 * @param res           Resource
 * @param releaseFunc   Resource release member function
 * @param execFunc      Resource acquisition member function
 * @param defCommit     Default reclamation method
 * @return
 */
template<typename RES, typename F_REL, typename F_EXEC>
GRaii makeRAII(RES &res, F_REL releaseFunc, F_EXEC execFunc, bool defCommit = true)
{
    static_assert(std::is_class<RES>::value, "RES is not a class or struct type.");
    static_assert(std::is_member_function_pointer<F_REL>::value, "F_REL is not a member function.");
    static_assert(std::is_member_function_pointer<F_EXEC>::value, "F_EXEC is not a member function.");
    assert(releaseFunc != nullptr && execFunc != nullptr);

    auto pRes = std::addressof(const_cast<typename no_const<RES>::type &>(res));
    return GRaii(std::bind(releaseFunc, pRes), std::bind(execFunc, pRes), defCommit);
}

/**
 * Creating RAII without a resource-creating version.
 *
 * @tparam RES          Resource type
 * @tparam F_REL        Member function address for releasing resources
 * @param res           Resource
 * @param releaseFunc   Resource release member function
 * @param defCommit     Default reclamation method
 * @return
 */
template<typename RES, typename F_REL>
GRaii makeRAII(RES &res, F_REL releaseFunc, bool defCommit = true)
{
    static_assert(std::is_class<RES>::value, "RES is not a class or struct type.");
    static_assert(std::is_member_function_pointer<F_REL>::value, "F_REL is not a member function.");
    assert(releaseFunc != nullptr);

    auto pRes = std::addressof(const_cast<typename no_const<RES>::type &>(res));
    return GRaii(std::bind(releaseFunc, pRes), [] {}, defCommit);
}

GX_NS_END

#endif //GX_GRAII_H
