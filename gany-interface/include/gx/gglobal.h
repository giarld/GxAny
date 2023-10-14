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

#ifndef GX_GGLOBAL_H
#define GX_GGLOBAL_H

#include <cstddef>
#include <cstdint>

#include "platform.h"


#if defined(BUILD_SHARED_LIBS)
    #define GX_BUILD_MODE_SHARED 1
    #define GX_BUILD_MODE_STATIC 0
#else
    #define GX_BUILD_MODE_SHARED 0
    #define GX_BUILD_MODE_STATIC 1
#endif


#if GX_BUILD_MODE_SHARED
#   if GX_PLATFORM_WINDOWS
#       define GX_DECL_EXPORT __declspec(dllexport)
#       define GX_DECL_IMPORT __declspec(dllimport)
#   else
#       define GX_DECL_EXPORT __attribute__((visibility("default")))
#       define GX_DECL_IMPORT
#   endif // GX_PLATFORM_WINDOWS
#else
#   define GX_DECL_EXPORT
#   define GX_DECL_IMPORT
#endif

#ifndef GX_API
#   define GX_API GX_DECL_EXPORT
#endif

#if GX_PLATFORM_WINDOWS
    #define GX_API_ATTR
    #define GX_API_CALL __stdcall
    #define GX_API_PTR  GX_API_CALL
#elif defined(__ANDROID__) && defined(__ARM_ARCH) && __ARM_ARCH >= 7 && defined(__ARM_32BIT_STATE)
    #define GX_API_ATTR __attribute__((pcs("aapcs-vfp")))
    #define GX_API_CALL
    #define GX_API_PTR  GX_API_ATTR
#else
    #define GX_API_ATTR
    #define GX_API_CALL
    #define GX_API_PTR
#endif


// functions
#define ARRAY_LEN(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))

#define GX_UNUSED(x) (void)x

#if __has_attribute(noinline)
#   define GX_NOINLINE __attribute__((noinline))
#else
#   define GX_NOINLINE
#endif

#if GX_COMPILER_MSVC
#    define GX_RESTRICT __restrict
#elif (defined(__clang__) || defined(__GNUC__))
#    define GX_RESTRICT __restrict__
#else
#    define GX_RESTRICT
#endif


#define GX_STRINGIZE(x) GX_STRINGIZE_(x)
#define GX_STRINGIZE_(x) #x

#define GX_NEW(Type, ...) new(std::nothrow) Type(__VA_ARGS__)

#define GX_DELETE(Obj)  \
while (Obj) {           \
    delete(Obj);        \
    Obj = nullptr;      \
} GX_UNUSED(0)

#endif //GX_GGLOBAL_H
