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

#ifndef GX_GANY_ITERATOR_H
#define GX_GANY_ITERATOR_H

#include "gx/gany.h"


GX_NS_BEGIN

class GAnyArrayIterator
{
public:
    using Type = std::vector<GAny>;
public:
    explicit GAnyArrayIterator(Type &array)
            : mArray(array)
    {
        mIter = mArray.begin();
        mOpIter = mArray.end();
    }

    bool hasNext() const
    {
        return mIter != mArray.end();
    }

    GAnyIteratorItem next()
    {
        if (mIter == mArray.end()) {
            return std::make_pair(nullptr, nullptr);
        }
        GAny v = *mIter;
        mOpIter = mIter;
        ++mIter;
        return std::make_pair((int32_t) std::distance(mArray.begin(), mOpIter), v);
    }

    void remove()
    {
        if (mOpIter != mArray.end()) {
            mIter = mArray.erase(mOpIter);
            mOpIter = mArray.end();
        }
    }

    bool hasPrevious() const
    {
        return mIter != mArray.begin();
    }

    GAnyIteratorItem previous()
    {
        if (mIter == mArray.begin()) {
            return std::make_pair(nullptr, nullptr);
        }
        --mIter;
        mOpIter = mIter;
        GAny v = *mIter;
        return std::make_pair((int32_t) std::distance(mArray.begin(), mOpIter), v);
    }

    void toFront()
    {
        mIter = mArray.begin();
        mOpIter = mArray.end();
    }

    void toBack()
    {
        mIter = mArray.end();
        mOpIter = mArray.end();
    }

private:
    Type &mArray;
    Type::iterator mIter;
    Type::iterator mOpIter;
};


class GAnyObjectIterator
{
public:
    using Type = std::unordered_map<std::string, GAny>;

public:
    explicit GAnyObjectIterator(Type &map)
            : mMap(map)
    {
        mIter = mMap.begin();
        mOpIter = mMap.end();
    }

    bool hasNext() const
    {
        return mIter != mMap.end();
    }

    GAnyIteratorItem next()
    {
        if (mIter == mMap.end()) {
            return std::make_pair(nullptr, nullptr);
        }
        std::pair<const std::string, GAny> v = *mIter;
        mOpIter = mIter;
        ++mIter;
        return std::make_pair(v.first, v.second);
    }

    void remove()
    {
        if (mOpIter != mMap.end()) {
            mIter = mMap.erase(mOpIter);
            mOpIter = mMap.end();
        }
    }

    void toFront()
    {
        mIter = mMap.begin();
        mOpIter = mMap.end();
    }

private:
    Type &mMap;
    Type::iterator mIter;
    Type::iterator mOpIter;
};

class GAnyObjectConstIterator
{
public:
    using Type = const std::unordered_map<std::string, GAny>;

public:
    explicit GAnyObjectConstIterator(Type &map)
            : mMap(map)
    {
        mIter = mMap.begin();
        mOpIter = mMap.end();
    }

    bool hasNext() const
    {
        return mIter != mMap.end();
    }

    GAnyIteratorItem next()
    {
        if (mIter == mMap.end()) {
            return std::make_pair(nullptr, nullptr);
        }
        std::pair<const std::string, GAny> v = *mIter;
        mOpIter = mIter;
        ++mIter;
        return std::make_pair(v.first, v.second);
    }

    void toFront()
    {
        mIter = mMap.begin();
        mOpIter = mMap.end();
    }

private:
    Type &mMap;
    Type::const_iterator mIter;
    Type::const_iterator mOpIter;
};

GX_NS_END

#endif //GX_GANY_ITERATOR_H
