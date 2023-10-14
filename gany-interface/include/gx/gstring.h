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

#ifndef GX_GSTRING_H
#define GX_GSTRING_H

#include "base.h"
#include "gglobal.h"

#include <string>
#include <vector>
#include <sstream>
#include <ostream>

#include <cstring>
#include <utility>
#include <algorithm>

#if GX_PLATFORM_WINDOWS

#include <windows.h>

#endif


GX_NS_BEGIN

#if GX_PLATFORM_WINDOWS

#define ENABLE_WSTRING  1

class GString;

class GX_API GWString
{
public:
    GWString();

    GWString(const wchar_t *wStr, int32_t size = -1);

    GWString(const std::wstring &wStr);

    GWString(const GString &str);

    GWString(const GWString &b);

    GWString(GWString &&b) noexcept;

    GWString &operator=(const GWString &b);

    GWString &operator=(GWString &&b) noexcept;

public:
    const wchar_t *data() const;

    int32_t length() const;

private:
    friend class GString;

    std::vector<wchar_t> mData;
};

#endif

/**
 * UTF-8 String
 */
class GX_API GString final
{
public:
    GString();

    GString(const char *str, int32_t size = -1);

#ifdef ENABLE_WSTRING

    GString(const GWString &wstring);

#endif

    GString(const GString &str);

    GString(GString &&str) noexcept;

    GString(char c);

    GString(std::string str);

    ~GString() = default;

public:

    /**
     * get ascii byte count
     * @return byte count
     */
    int32_t count() const;

    /**
     * get utf-8 char length
     * @return char length
     */
    int32_t length() const;

    const char *data() const;

    const char *c_str() const;

    GString at(int32_t index) const;

    std::string toStdString() const;

    void reset(const char *str, int32_t size = -1);

    void reset(const GString &str);

    void reset(const std::string &str);

    int compare(const GString &bStr) const;

    GString &append(const GString &str);

    GString &insert(int32_t index, const GString &str);

    bool isEmpty() const;

    bool startWith(const GString &str) const;

    bool endWith(const GString &str) const;

    GString left(int32_t n) const;

    GString right(int32_t n) const;

    /**
     * Substring the string; when len is set to the default value, it is taken from begin to the end.
     * @param begin
     * @param len
     * @return
     */
    GString substring(int32_t begin, int32_t len = -1) const;

    /**
     * Replace the string
     * @param before    The substring to be replaced
     * @param after     Replace with
     * @param begin     Starting from the 'begin'th character
     * @return
     */
    GString replace(const GString &before, const GString &after, int32_t begin = 0) const;

    std::vector<GString> split(const GString &cs) const;

    /**
     * Forward search, return the starting position of the target string if found, or -1 if not found.
     * @param str
     * @param from
     * @return
     */
    int32_t indexOf(const GString &str, int32_t from = 0) const;

    /**
     * Reverse search, return the starting position of the target string if found, or -1 if not found.
     * @param str
     * @param from  Positive numbers indicate positions starting from the beginning, where 0 represents the first position,
     *              and negative numbers indicate positions starting from the end, where -1 represents the last position.
     * @return
     */
    int32_t lastIndexOf(const GString &str, int32_t from = -1) const;

    void swap(GString &b);

    /**
     * Retrieve the Unicode of the specified character.
     * @param index
     * @return
     */
    uint32_t codepoint(int32_t index) const;

#ifdef ENABLE_WSTRING

    GWString toUtf16() const;

#endif

    GString toUpper() const;

    GString toLower() const;

    GString &operator=(const GString &bStr);

    GString &operator=(GString &&bStr) noexcept;

    bool operator==(const GString &bStr) const;

    bool operator>(const GString &bStr) const;

    bool operator<(const GString &bStr) const;

    bool operator>=(const GString &bStr) const;

    bool operator<=(const GString &bStr) const;

    bool operator!=(const GString &bStr) const;

    GString &operator+=(const GString &str);

    explicit operator std::string() const;

    GString &operator<<(const GString &b);

    GString &operator<<(char c);

    template<class T>
    friend GString &operator<<(GString &a, const T &b);

    friend std::ostream &operator<<(std::ostream &os, const GString &string);

    friend std::istream &operator>>(std::istream &is, GString &string);

    friend GString operator+(const GString &a, const GString &b);

    GString arg(const GString &key, char c) const;

    GString arg(const GString &key, const char *a) const;

    GString arg(const GString &key, bool b) const;

    GString arg(const GString &key, const GString &a) const;

    template<typename T>
    GString arg(const GString &key, T a) const
    {
        return arg(key, GString::toString(a));
    }

    GString arg(const GString &key, const std::string &a) const;

    GString arg(char c) const;

    GString arg(const char *str) const;

    GString arg(const GString &str) const;

    GString arg(bool b) const;

    GString arg(const std::string &s) const;

    template<typename T>
    GString arg(T a) const
    {
        return arg(GString::toString(a));
    }

    std::string toString() const
    {
        return toStdString();
    }

public: // static
    template<class T>
    static GString toString(T num)
    {
        std::ostringstream o;
        o << num;
        return o.str();
    }

    static GString toString(bool num)
    {
        std::ostringstream o;
        o << (num ? "true" : "false");
        return o.str();
    }

    static GString fromCodepoint(uint32_t codepoint);

private:
    void _build(const char *str, int32_t size = -1);

    void _build(const GString &str);

    void _build(char c);

    void _clear();

    /**
     * Retrieve the starting pointer position of the next character.
     * @param pIndex Starting pointer position.
     * @return
     */
    int32_t _next(int32_t pIndex) const;

    /**
     * Obtain the ASCII character index corresponding to the number of Unicode characters.
     * @param uniCharSize
     * @return
     */
    int32_t _seek(int32_t uniCharSize) const;

    /**
     * Calculate the number of Unicode characters.
     * @return
     */
    int32_t _checkLength();

    GString _atChar(int32_t index) const;

    void _replace(const GString &before, const GString &after, int32_t offset);

private:
    friend struct std::hash<gx::GString>;

    std::string mStr;
    int32_t mLength = 0;
};

inline GString operator+(const GString &a, const GString &b)
{
    GString temp = a;
    temp.append(b);
    return temp;
}

inline std::ostream &operator<<(std::ostream &os, const GString &string)
{
    os << string.mStr;
    return os;
}

template<class T>
inline GString &operator<<(GString &a, const T &b)
{
    a.append(GString::toString(b));
    return a;
}

inline std::istream &operator>>(std::istream &is, GString &string)
{
    std::string temp;
    is >> temp;
    string.reset(temp);

    return is;
}


inline GString::GString()
{
    _build("");
}

inline GString::GString(const char *str, int32_t size)
{
    if (!str) {
        _build("");
    } else {
        _build(str, size);
    }
}

#ifdef ENABLE_WSTRING

inline GString::GString(const GWString &wstr)
{
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int) wstr.length(), nullptr, 0, nullptr, nullptr);
    std::vector<char> utf8(len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int) wstr.length(), utf8.data(), len, nullptr, nullptr);
    utf8[len] = '\0';
    _build(utf8.data());
}

#endif

inline GString::GString(const GString &str)
        : mStr(str.mStr)
{
    _checkLength();
}

inline GString::GString(GString &&str) noexcept
        : mStr(std::move(str.mStr)),
          mLength(str.mLength)
{
    str.mLength = 0;
}

inline GString::GString(char c)
{
    _build(c);
}

inline GString::GString(std::string str)
        : mStr(std::move(str))
{
    _checkLength();
}

inline int32_t GString::count() const
{
    return (int32_t) mStr.size();
}

inline int32_t GString::length() const
{
    return mLength;
}

inline const char *GString::data() const
{
    return mStr.data();
}

inline const char *GString::c_str() const
{
    return mStr.c_str();
}

inline GString GString::at(int32_t index) const
{
    return _atChar((int32_t) index);
}

inline std::string GString::toStdString() const
{
    return mStr;
}

inline void GString::reset(const char *str, int32_t size)
{
    _build(str, size);
}

inline void GString::reset(const GString &str)
{
    _build(str);
}

inline void GString::reset(const std::string &str)
{
    mStr = str;
    _checkLength();
}

inline int GString::compare(const GString &bStr) const
{
    return mStr.compare(bStr.mStr);
}

inline GString &GString::append(const GString &str)
{
    mStr.append(str.mStr);
    _checkLength();
    return *this;
}

inline GString &GString::insert(int32_t index, const GString &str)
{
    if (index >= length()) {
        return this->append(str);
    }
    GString newStr = this->left(index) + str + this->substring(index);
    this->swap(newStr);
    return *this;
}

inline bool GString::isEmpty() const
{
    return mStr.empty();
}

inline bool GString::startWith(const GString &str) const
{
    if (str.count() == 0 && this->count() == 0) {
        return true;
    }
    if (str.count() > this->count()) {
        return false;
    }
    return memcmp(this->data(), str.data(), str.count()) == 0;
}

inline bool GString::endWith(const GString &str) const
{
    if (str.count() == 0 && this->count() == 0) {
        return true;
    }
    if (str.count() > this->count()) {
        return false;
    }
    return memcmp(this->data() + this->count() - str.count(), str.data(), str.count()) == 0;
}

inline GString GString::left(int32_t n) const
{
    if (n == 0) {
        return "";
    }

    if (n >= this->length()) {
        return *this;
    }

    int32_t i = _seek((int32_t) n);
    return {this->data(), i};
}

inline GString GString::right(int32_t n) const
{
    if (n == 0) {
        return "";
    }

    int32_t sSize = this->length();
    if (n > sSize) {
        return *this;
    }

    int32_t i = _seek((int32_t) (sSize - n));
    return {this->data() + i, (int32_t) (this->count() - i)};
}

inline GString GString::substring(int32_t begin, int32_t len) const
{
    int32_t b = _seek((int32_t) begin);
    int32_t e;
    if (len < 0) {
        e = (int32_t) this->count();
    } else {
        e = _seek((int32_t) (begin + len));
    }
    if (e <= b) {
        return "";
    }
    return {this->data() + b, e - b};
}

inline GString GString::replace(const GString &before, const GString &after, int32_t begin) const
{
    GString temp = *this;
    temp._replace(before, after, _seek((int32_t) begin));

    return temp;
}

inline std::vector<GString> GString::split(const gx::GString &cs) const
{
    std::vector<GString> clips;
    int32_t begin = 0;
    for (int32_t i = 0; i < this->count() && i + cs.count() <= this->count();) {
        if (memcmp(this->data() + i, cs.data(), (size_t) cs.count()) == 0) {
            clips.emplace_back(this->data() + begin, i - begin);
            i += cs.count();
            begin = i;
        } else {
            i = _next(i);
        }
    }
    if (begin < this->count()) {
        clips.emplace_back(this->data() + begin, this->count() - begin);
    }
    return clips;
}

inline int32_t GString::indexOf(const GString &str, int32_t from) const
{
    if (from < 0) {
        from = 0;
    }
    if (from >= this->length()) {
        return -1;
    }
    int32_t tSize = from;
    auto len = (int32_t) count();
    int32_t i = _seek(tSize);
    while (i < len) {
        if (memcmp(this->data() + i, str.data(), str.count()) == 0) {
            return tSize;
        }
        i = (int32_t) _next(i);
        ++tSize;
    }
    return -1;
}

inline int32_t GString::lastIndexOf(const GString &str, int32_t from) const
{
    auto sourceLen = (int32_t) this->length();
    auto targetLen = (int32_t) str.length();
    if (from < 0) {
        from = sourceLen + from;
    }
    int32_t rightIndex = sourceLen - targetLen;
    if (from > rightIndex) {
        from = rightIndex;
    }
    if (from < 0) {
        return -1;
    }
    if (targetLen == 0) {
        return -1;
    }
    GString targetFirstChar = str.at(0);
    int32_t i = from;
    while (i >= 0) {
        while (i >= 0 && this->_atChar(i) != targetFirstChar) {
            i--;
        }
        if (i < 0) {
            return -1;
        }
        if (this->substring(i, targetLen) == str) {
            return i;
        }
        i--;
    }
    return -1;
}

inline void GString::swap(GString &b)
{
    std::swap(this->mStr, b.mStr);
    std::swap(this->mLength, b.mLength);
}

inline uint32_t GString::codepoint(int32_t index) const
{
    GString c = this->at(index);
    const char *u = c.data();
    size_t l = c.count();
    if (l < 1) {
        return 0;
    }
    unsigned char u0 = u[0];
    if (u0 >= 0 && u0 <= 127) {
        return u0;
    }
    if (l < 2) {
        return 0;
    }
    unsigned char u1 = u[1];
    if (u0 >= 192 && u0 <= 223) {
        return (u0 - 192) * 64 + (u1 - 128);
    }
    if ((uint8_t) (u[0]) == 0xed && (u[1] & 0xa0) == 0xa0) {
        return 0;
    } //code points, 0xd800 to 0xdfff
    if (l < 3) {
        return 0;
    }
    unsigned char u2 = u[2];
    if (u0 >= 224 && u0 <= 239) {
        return (u0 - 224) * 4096 + (u1 - 128) * 64 + (u2 - 128);
    }
    if (l < 4) {
        return 0;
    }
    unsigned char u3 = u[3];
    if (u0 >= 240 && u0 <= 247) {
        return (u0 - 240) * 262144 + (u1 - 128) * 4096 + (u2 - 128) * 64 + (u3 - 128);
    }
    return 0;
}

#ifdef ENABLE_WSTRING

inline GWString GString::toUtf16() const
{
    return {*this};
}

#endif

inline GString GString::toUpper() const
{
    GString temp;
    for (int32_t i = 0; i < length(); i++) {
        GString c = at(i);
        if (c.count() == 1 && c >= 'a' && c <= 'z') {
            c = char(c.data()[0] - ('a' - 'A'));
        }
        temp += c;
    }
    return temp;
}

inline GString GString::toLower() const
{
    GString temp;
    for (int32_t i = 0; i < length(); i++) {
        GString c = at(i);
        if (c.count() == 1 && c >= 'A' && c <= 'Z') {
            c = char(c.data()[0] + ('a' - 'A'));
        }
        temp += c;
    }
    return temp;
}

inline GString &GString::operator=(const GString &bStr)
{
    if (&bStr == this) {
        return *this;
    }
    _build(bStr);
    return *this;
}

inline GString &GString::operator=(GString &&bStr) noexcept
{
    if (&bStr == this) {
        return *this;
    }
    std::swap(this->mStr, bStr.mStr);
    std::swap(this->mLength, bStr.mLength);
    return *this;
}

inline bool GString::operator==(const GString &bStr) const
{
    return this->compare(bStr) == 0;
}

inline bool GString::operator>(const GString &bStr) const
{
    return this->compare(bStr) > 0;
}

inline bool GString::operator<(const GString &bStr) const
{
    return this->compare(bStr) < 0;
}

inline bool GString::operator>=(const GString &bStr) const
{
    return this->compare(bStr) >= 0;
}

inline bool GString::operator<=(const GString &bStr) const
{
    return this->compare(bStr) <= 0;
}

inline bool GString::operator!=(const GString &bStr) const
{
    return this->compare(bStr) != 0;
}

inline GString &GString::operator+=(const GString &str)
{
    return this->append(str);
}

inline GString::operator std::string() const
{
    return toStdString();
}

inline GString &GString::operator<<(const GString &b)
{
    this->append(b);
    return *this;
}

inline GString &GString::operator<<(char c)
{
    this->append(GString(c));
    return *this;
}

inline GString GString::arg(const GString &key, char c) const
{
    GString keys = "{";
    keys += key;
    keys += "}";
    GString newStr = replace(keys, GString(c));
    return newStr;
}

inline GString GString::arg(const GString &key, const char *a) const
{
    return arg(key, GString(a));
}

inline GString GString::arg(const GString &key, bool b) const
{
    return arg(key, GString(b ? "true" : "false"));
}

inline GString GString::arg(const GString &key, const GString &a) const
{
    GString keys = "{";
    keys += key;
    keys += "}";
    GString newStr = replace(keys, a);
    return newStr;
}

inline GString GString::arg(const GString &key, const std::string &a) const
{
    GString keys = "{";
    keys += key;
    keys += "}";
    GString newStr = replace(keys, a);
    return newStr;
}

inline GString GString::arg(char c) const
{
    return arg(GString(c));
}

inline GString GString::arg(const char *str) const
{
    return arg(GString(str));
}

inline GString GString::arg(const GString &str) const
{
    GString headC = "{";
    GString endC = "}";
    int32_t begin = -1;
    int32_t end = -1;
    int32_t len = mLength;
    for (int32_t i = 0; i < len; ++i) {
        GString c = _atChar(i);
        if (begin < 0) {
            if (c == headC) {
                begin = i;
            }
        } else {
            if (c == endC) {
                end = i;
                break;
            }
        }
    }
    if (end <= 0) {
        return *this;
    }
    return substring(0, begin) + str + substring(end + 1);
}

inline GString GString::arg(bool b) const
{
    return arg((b ? "true" : "false"));
}

inline GString GString::arg(const std::string &s) const
{
    return arg(GString(s));
}

//=============== Begin Static ================//

inline GString GString::fromCodepoint(uint32_t codepoint)
{
    uint8_t c[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    if (codepoint <= 0x7F) {
        c[0] = codepoint;
    } else if (codepoint <= 0x7FF) {
        c[0] = (codepoint >> 6) + 192;
        c[1] = (codepoint & 63) + 128;
    } else if (0xd800 <= codepoint && codepoint <= 0xdfff) {} //invalid block of utf8
    else if (codepoint <= 0xFFFF) {
        c[0] = (codepoint >> 12) + 224;
        c[1] = ((codepoint >> 6) & 63) + 128;
        c[2] = (codepoint & 63) + 128;
    } else if (codepoint <= 0x10FFFF) {
        c[0] = (codepoint >> 18) + 240;
        c[1] = ((codepoint >> 12) & 63) + 128;
        c[2] = ((codepoint >> 6) & 63) + 128;
        c[3] = (codepoint & 63) + 128;
    }

    return {(const char *) c, (int32_t) strlen((char *) c)};
}

//=============== End Static ================//

inline void GString::_build(const char *str, int32_t size)
{
    size_t length = size >= 0 ? std::min((size_t) size, strlen(str)) : strlen(str);

    mStr = std::string(str, length);
    _checkLength();
}

inline void GString::_build(const GString &str)
{
    mStr = str.mStr;
    _checkLength();
}

inline void GString::_build(char c)
{
    mStr = c;
    _checkLength();
}

inline void GString::_clear()
{
    mStr.clear();
    _checkLength();
}

inline int32_t GString::_next(int32_t pIndex) const
{
    int32_t i = pIndex;
    if (i >= count()) {
        return (int32_t) count();
    }
    auto c = (unsigned char) mStr[i];
    int32_t ts;
    if (c < 0x80) {
        ts = 1;
    } else if (c < 0xe0) {
        ts = 2;
    } else if (c < 0xf0) {
        ts = 3;
    } else if (c < 0xf8) {
        ts = 4;
    } else if (c < 0xfc) {
        ts = 5;
    } else if (c < 0xfe) {
        ts = 6;
    } else {
        ts = 7;
    }
    i += ts;
    if (i > this->count()) {
        i = this->count();
    }
    return i;
}

inline int32_t GString::_seek(int32_t uniCharSize) const
{
    if (uniCharSize <= 0) {
        return 0;
    }
    int32_t tSize = 0;
    int32_t i = 0;
    for (; i < this->count() && tSize < uniCharSize;) {
        i = _next(i);
        tSize++;
    }
    if (i > this->count()) {
        i = this->count();
    }
    return (int32_t) i;
}

inline int32_t GString::_checkLength()
{
    int32_t tSize = 0;
    int32_t len = count();
    int32_t i = 0;
    while (i < len) {
        i = _next(i);
        ++tSize;
    }
    mLength = tSize;
    return tSize;
}

inline GString GString::_atChar(int32_t index) const
{
    int32_t i = _seek(index);
    int32_t ts = (int32_t) _next(i) - i;
    if (ts <= 0) {
        return {};
    }
    return {data() + i, ts};
}

inline void GString::_replace(const GString &before, const GString &after, int32_t offset)
{
    if (before.isEmpty()) {
        return;
    }
    if (before == after) {
        return;
    }

    auto beginOff = (int32_t) mStr.find(before.mStr, offset);
    if ((size_t)beginOff == std::string::npos) {
        return;
    }

    size_t end = beginOff + before.count();

    std::string temp = mStr;
    mStr = temp.substr(0, beginOff) + after.mStr + temp.substr(end, std::string::npos);
    _checkLength();

    _replace(before, after, beginOff + after.count());
}


#ifdef ENABLE_WSTRING

inline GWString::GWString()
        : mData(1)
{
}

inline GWString::GWString(const wchar_t *wStr, int32_t size)
{
    if (size <= 0) {
        size = (int32_t) wcslen(wStr);
    }
    mData.resize(size + 1);
    memcpy(mData.data(), wStr, sizeof(wchar_t) * size);
    mData[size] = L'\0';
}

inline GWString::GWString(const std::wstring &wStr)
{
    mData.resize(wStr.size() + 1);
    memcpy(mData.data(), wStr.data(), sizeof(wchar_t) * wStr.size());
    mData[wStr.size()] = L'\0';
}

inline GWString::GWString(const GString &str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int) str.count(), nullptr, 0);
    mData.resize(len + 1);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int) str.count(), mData.data(), len);
    mData[len] = L'\0';
}

inline GWString::GWString(const GWString &b) = default;

inline GWString::GWString(GWString &&b) noexcept
        : mData(std::move(b.mData))
{
}

inline GWString &GWString::operator=(const GWString &b)
{
    if (this != &b) {
        mData = b.mData;
    }
    return *this;
}

inline GWString &GWString::operator=(GWString &&b) noexcept
{
    if (this != &b) {
        std::swap(mData, b.mData);
    }
    return *this;
}

inline const wchar_t *GWString::data() const
{
    return mData.data();
}

inline int32_t GWString::length() const
{
    return (int32_t) mData.size() - 1;
}

#endif //ENABLE_WSTRING

GX_NS_END

namespace std
{

template<>
struct hash<gx::GString>
{
    size_t operator()(const gx::GString &type) const
    {
        std::hash<string> sHashFunc;
        return sHashFunc(type.mStr);
    }
};
}

#endif //GX_GSTRING_H
