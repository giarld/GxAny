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

#ifndef GX_COMMON_H
#define GX_COMMON_H

#include "base.h"

#include <type_traits>
#include <functional>

GX_NS_BEGIN

/**
 * Compare two variables of the same type bitwise for equality.
 *
 * @tparam T
 * @param l
 * @param r
 * @return
 */
template<typename T>
bool bitwiseEqual(const T &l, const T &r)
{
    using U =
    std::conditional_t<alignof(T) == 1,
            std::uint8_t,
            std::conditional_t<alignof(T) == 2,
                    std::uint16_t,
                    std::conditional_t<alignof(T) == 4,
                            std::uint32_t,
                            std::uint64_t
                    >
            >
    >;

    const U *uL = reinterpret_cast<const U *>(&l);
    const U *uR = reinterpret_cast<const U *>(&r);
    for (size_t i = 0; i < sizeof(T) / sizeof(U); i++, uL++, uR++) {
        if (*uL != *uR)
            return false;
    }

    return true;
}


template<typename T>
void hashCombine(size_t& s, const T& v)
{
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

/**
 * Compute the hash and merge it into the result of the previous hash.
 * @tparam T
 * @param last  previous hash
 * @param v
 * @return
 */
template<typename T>
size_t hashOf(size_t last, const T& v)
{
    using U =
    std::conditional_t<alignof(T) == 1,
            std::uint8_t,
            std::conditional_t<alignof(T) == 2,
                    std::uint16_t,
                    std::conditional_t<alignof(T) == 4,
                            std::uint32_t,
                            std::uint64_t
                    >
            >
    >;

    for (size_t i = 0; i < sizeof(T) / sizeof(U); i++) {
        hashCombine(last, reinterpret_cast<const U *>(&v)[i]);
    }

    return last;
}

/**
 * Compute the hash.
 * @tparam T
 * @param v
 * @return
 */
template<typename T>
size_t hashOf(const T& v)
{
    return hashOf(0, v);
}

/**
 * Calculate the hash value of a block with 1-byte alignment.
 * @param data  data head ptr
 * @param len   block length
 * @return
 */
inline size_t hashOfByte(const void *data, size_t len)
{
    const auto *tData = (const uint8_t*)data;
    size_t hash = 0;
    for (size_t i = 0; i < len; i++) {
        hashCombine(hash, tData[i]);
    }
    return hash;
}

/**
 * Merge hash values.
 * @param master    main hash value
 * @param branch    branch hash value
 * @return
 */
inline size_t hashMerge(size_t master, size_t branch)
{
    master ^= branch + 0x9e3779b9 + (master << 6) + (master >> 2);
    return master;
}

GX_NS_END

#endif //GX_COMMON_H
