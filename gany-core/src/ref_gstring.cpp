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

#include "gx/gstring.h"
#include "gx/gany.h"


GX_NS_BEGIN

void refGString()
{
    Class<GString>("Gx", "GString", "Gx string, A UTF-8 string and a series of processing algorithms for UTF-8.")
            .construct<>()
            .construct<const char *>()
            .construct<const char *, int32_t>()
#if GX_PLATFORM_WINDOWS
            .construct<const GWString &>("Create from wide string (Windows platforms only).")
#endif
            .construct<const GString &>()
            .construct<char>()
            .construct<std::string>()
            .func("count", &GString::count, "Gets the byte length of the string.")
            .func("length", &GString::length, "Get string length (number of words).")
            .func("data", &GString::data, "Back buffer header pointer.")
            .func("c_str", &GString::c_str, "Back buffer header pointer.")
            .func("at", [](const GString &self, int32_t index) {
                return self.at(index);
            }, "Gets the character at the specified position.")
            .func("toStdString", &GString::toStdString, "Convert to std:: string.")
            .func("reset", [](GString &self, const char *str) {
                self.reset(str);
            }, "Reset to specified string.")
            .func("reset", [](GString &self, const char *str, int32_t size) {
                self.reset(str, size);
            }, "Reset to specified string.")
            .func("reset", [](GString &self, const GString &str) {
                self.reset(str);
            }, "Reset to specified string.")
            .func("reset", [](GString &self, const std::string &str) {
                self.reset(str);
            }, "Reset to specified string.")
            .func("compare", &GString::compare, "Compare whether two strings are the same.")
            .func("compare", [](GString &self, const std::string &b) {
                return self.compare(b);
            }, "Compare whether two strings are the same.")
            .func("append", &GString::append, "Add a string at the end of the string.")
            .func("append", [](GString &self, const std::string &b) {
                return self.append(b);
            }, "Add a string at the end of the string.")
            .func("insert", &GString::insert, "Inserts a string(arg2) at the specified position(arg1).")
            .func("insert", [](GString &self, int32_t index, const std::string &b) {
                return self.insert(index, b);
            }, "Inserts a string(arg2) at the specified position(arg1).")
            .func("isEmpty", &GString::isEmpty, "Check if the string is empty.")
            .func("startWith", &GString::startWith, "Check whether the string starts with the specified string.")
            .func("startWith", [](GString &self, const std::string &b) {
                return self.startWith(b);
            }, "Check whether the string starts with the specified string.")
            .func("endWith", &GString::endWith, "Checks whether the string ends with the specified string.")
            .func("endWith", [](GString &self, const std::string &b) {
                return self.endWith(b);
            }, "Checks whether the string ends with the specified string.")
            .func("left", &GString::left, "Intercepts the left specified length string.")
            .func("right", &GString::right, "Intercepts the right specified length string.")
            .func("substring", &GString::substring, "Intercept the string with length len (arg2) from begin (arg1).")
            .func("substring", [](GString &self, int32_t begin) {
                return self.substring(begin);
            }, "Intercept the string with length len (arg2) from begin (arg1).")
            .func("replace", [](GString &self, const GString &before, const GString &after, int32_t begin) {
                return self.replace(before, after, begin);
            }, "Search from begin (arg3) and replace all before (arg1) in the string with after (arg2).")
            .func("replace", [](GString &self, const std::string &before, const std::string &after, int32_t begin) {
                return self.replace(before, after, begin);
            }, "Search from begin (arg3) and replace all before (arg1) in the string with after (arg2).")
            .func("replace", [](GString &self, const GString &before, const GString &after) {
                return self.replace(before, after);
            }, "Search from 0 and replace all before (arg1) in the string with after (arg2).")
            .func("replace", [](GString &self, const std::string &before, const std::string &after) {
                return self.replace(before, after);
            }, "Search from 0 and replace all before (arg1) in the string with after (arg2).")
            .func("split", &GString::split, "Cut the string with the specified string as the separator.")
            .func("split", [](GString &self, const std::string &cs) {
                return self.split(cs);
            }, "Cut the string with the specified string as the separator.")
            .func("indexOf", [](GString &self, const GString &str, int32_t from) {
                return self.indexOf(str, from);
            }, "Find the position subscript of the first matching str (arg1) starting from (arg2).")
            .func("indexOf", [](GString &self, const std::string &str, int32_t from) {
                return self.indexOf(str, from);
            }, "Find the position subscript of the first matching str (arg1) starting from (arg2).")
            .func("indexOf", [](GString &self, const GString &str) {
                return self.indexOf(str);
            }, "Find the position subscript of the first matching str (arg1) starting from 0.")
            .func("indexOf", [](GString &self, const std::string &str) {
                return self.indexOf(str);
            }, "Find the position subscript of the first matching str (arg1) starting from 0.")
            .func("lastIndexOf", [](GString &self, const GString &str, int32_t from) {
                     return self.lastIndexOf(str, from);
                 },
                 "Reverse search the position subscript of the first matching str (arg1) from (arg2). "
                 "When the parameter (from) arg2 is a positive number, it indicates the position from the beginning. "
                 "For example, 0 indicates the first position, a negative number indicates the position from the end, "
                 "for example, -1 indicates the last position.")
            .func("lastIndexOf", [](GString &self, const std::string &str, int32_t from) {
                return self.lastIndexOf(str, from);
            })
            .func("lastIndexOf", [](GString &self, const GString &str) {
                return self.lastIndexOf(str);
            })
            .func("lastIndexOf", [](GString &self, const std::string &str) {
                return self.lastIndexOf(str);
            })
            .func("swap", &GString::swap)
            .func("codepoint", [](GString &self, int32_t index) {
                return (int32_t) self.codepoint(index);
            }, "Get Unicode of the specified character.")
#if GX_PLATFORM_WINDOWS
            .func("toUtf16", &GString::toUtf16, "Convert string to UTF-16, windows platform only.")
#endif
            .func("toUpper", &GString::toUpper, "Converts all lowercase letters in a string to uppercase letters.")
            .func("toLower", &GString::toLower, "Converts all uppercase letters in a string to lowercase letters.")
            .func("arg", [](GString &self, const GAny &value) {
                return self.arg(value.toString());
            }, "Replace the string of '{}' area with the string of arg1.")
            .func("arg", [](GString &self, const std::string &key, const GAny &value) {
                return self.arg(key, value.toString());
            }, "Replace the string of '{arg1}' area with the string of arg2.")
            .staticFunc("toString", [](GAny &value) {
                return GString(value.toString());
            }, "Convert any type to GString, as long as the original MetaFunction::ToString of this type is valid.")
            .staticFunc("fromCodepoint", [](int32_t codepoint) {
                return GString::fromCodepoint(codepoint);
            }, "Create a string from Unicode.")
            .func(MetaFunction::ToString, [](const GString &self) {
                return self.toString();
            })
            .func(MetaFunction::Addition, [](const GString &self, const GString &b) {
                return self + b;
            })
            .func(MetaFunction::Length, [](const GString &self) {
                return (size_t) self.length();
            })
            .func(MetaFunction::EqualTo, &GString::operator==)
            .func(MetaFunction::LessThan, &GString::operator<)
            .func(MetaFunction::GetItem, [](const GString &self, int32_t index) {
                return self.at(index);
            });

#if GX_PLATFORM_WINDOWS
    Class<GWString>("Gx", "GWString", "Gx wstring.")
            .construct<>()
            .construct<const wchar_t *>()
            .construct<const wchar_t *, int32_t>()
            .construct<const std::wstring &>()
            .construct<const GString &>()
            .construct<const GWString &>()
            .func("data", &GWString::data)
            .func("length", &GWString::length);
#endif
}

GX_NS_END
