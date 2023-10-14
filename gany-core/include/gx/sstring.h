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

#ifndef GX_STATIC_STRING_H
#define GX_STATIC_STRING_H

#include "gx/common.h"

#include <utility>


GX_NS_BEGIN

class StaticString
{
public:
    StaticString(const char *str = "")
        : mStr(str)
    {}

    StaticString(const StaticString &rh) = default;

    StaticString(StaticString &&rh) noexcept
    {
        std::swap(mStr, rh.mStr);
    }

public:
    const char *c_str() const
    {
        return mStr;
    }

    const char *data() const
    {
        return mStr;
    }

    size_t size() const
    {
        return strlen(mStr);
    }

    friend void swap(StaticString &lh, StaticString &rh)
    {
        std::swap(lh.mStr, rh.mStr);
    }

    friend bool operator==(const StaticString &lh, const StaticString &rh)
    {
        return strcmp(lh.mStr, rh.mStr) == 0;
    }

    friend bool operator<(const StaticString &lh, const StaticString &rh)
    {
        return strcmp(lh.mStr, rh.mStr) < 0;
    }

private:
    const char *mStr = nullptr;
};

GX_NS_END

namespace std
{

template<>
struct hash<gx::StaticString>
{
    size_t operator()(const gx::StaticString &type) const
    {
        return gx::hashOfByte(type.data(), type.size());
    }
};

}

#endif //GX_STATIC_STRING_H
