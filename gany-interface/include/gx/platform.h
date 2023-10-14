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

#ifndef GX_PLATFORM_H
#define GX_PLATFORM_H

// Architecture
#define GX_ARCH_32BIT 0
#define GX_ARCH_64BIT 0

// Compiler
#define GX_COMPILER_CLANG          0
#define GX_COMPILER_CLANG_ANALYZER 0
#define GX_COMPILER_GCC            0
#define GX_COMPILER_MSVC           0

// Endianess
#define GX_CPU_ENDIAN_BIG    0
#define GX_CPU_ENDIAN_LITTLE 0

// CPU
#define GX_CPU_ARM   0
#define GX_CPU_JIT   0
#define GX_CPU_MIPS  0
#define GX_CPU_PPC   0
#define GX_CPU_RISCV 0
#define GX_CPU_X86   0

// C Runtime
#define GX_CRT_BIONIC 0
#define GX_CRT_GLIBC  0
#define GX_CRT_LIBCXX 0
#define GX_CRT_MINGW  0
#define GX_CRT_MSVC   0
#define GX_CRT_NEWLIB 0

#ifndef GX_CRT_MUSL
#	define GX_CRT_MUSL 0
#endif // GX_CRT_MUSL

#ifndef GX_CRT_NONE
#	define GX_CRT_NONE 0
#endif // GX_CRT_NONE

// Platform
#define GX_PLATFORM_ANDROID    0
#define GX_PLATFORM_BSD        0
#define GX_PLATFORM_EMSCRIPTEN 0
#define GX_PLATFORM_HURD       0
#define GX_PLATFORM_IOS        0
#define GX_PLATFORM_LINUX      0
#define GX_PLATFORM_NX         0
#define GX_PLATFORM_OSX        0
#define GX_PLATFORM_PS4        0
#define GX_PLATFORM_RPI        0
#define GX_PLATFORM_STEAMLINK  0
#define GX_PLATFORM_WINDOWS    0
#define GX_PLATFORM_UWP        0
#define GX_PLATFORM_GDK        0

#ifdef NDEBUG
#   define GX_DEBUG 0
#else
#   define GX_DEBUG 1
#endif

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers
#if defined(__clang__)
// clang defines __GNUC__ or _MSC_VER
#	undef  GX_COMPILER_CLANG
#	define GX_COMPILER_CLANG (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#	if defined(__clang_analyzer__)
#		undef  GX_COMPILER_CLANG_ANALYZER
#		define GX_COMPILER_CLANG_ANALYZER 1
#	endif // defined(__clang_analyzer__)
#elif defined(_MSC_VER)
#	undef  GX_COMPILER_MSVC
#	define GX_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
#	undef  GX_COMPILER_GCC
#	define GX_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#	error "GX_COMPILER_* is not defined!"
#endif //

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Architectures
#if defined(__arm__)     \
 || defined(__aarch64__) \
 || defined(_M_ARM)
#	undef  GX_CPU_ARM
#	define GX_CPU_ARM 1
#	define GX_CACHE_LINE_SIZE 64
#elif defined(__MIPSEL__)     \
 || defined(__mips_isa_rev) \
 || defined(__mips64)
#	undef  GX_CPU_MIPS
#	define GX_CPU_MIPS 1
#	define GX_CACHE_LINE_SIZE 64
#elif defined(_M_PPC)        \
 || defined(__powerpc__)   \
 || defined(__powerpc64__)
#	undef  GX_CPU_PPC
#	define GX_CPU_PPC 1
#	define GX_CACHE_LINE_SIZE 128
#elif defined(__riscv)   \
 || defined(__riscv__) \
 || defined(RISCVEL)
#	undef  GX_CPU_RISCV
#	define GX_CPU_RISCV 1
#	define GX_CACHE_LINE_SIZE 64
#elif defined(_M_IX86)    \
 || defined(_M_X64)     \
 || defined(__i386__)   \
 || defined(__x86_64__)
#	undef  GX_CPU_X86
#	define GX_CPU_X86 1
#	define GX_CACHE_LINE_SIZE 64
#else // PNaCl doesn't have CPU defined.
#	undef  GX_CPU_JIT
#	define GX_CPU_JIT 1
#	define GX_CACHE_LINE_SIZE 64
#endif //

#if defined(__x86_64__)    \
 || defined(_M_X64)        \
 || defined(__aarch64__)   \
 || defined(__64BIT__)     \
 || defined(__mips64)      \
 || defined(__powerpc64__) \
 || defined(__ppc64__)     \
 || defined(__LP64__)
#	undef  GX_ARCH_64BIT
#	define GX_ARCH_64BIT 64
#else
#	undef  GX_ARCH_32BIT
#	define GX_ARCH_32BIT 32
#endif //

#if GX_CPU_PPC
// _LITTLE_ENDIAN exists on ppc64le.
#	if _LITTLE_ENDIAN
#		undef  GX_CPU_ENDIAN_LITTLE
#		define GX_CPU_ENDIAN_LITTLE 1
#	else
#		undef  GX_CPU_ENDIAN_BIG
#		define GX_CPU_ENDIAN_BIG 1
#	endif
#else
#	undef  GX_CPU_ENDIAN_LITTLE
#	define GX_CPU_ENDIAN_LITTLE 1
#endif // GX_PLATFORM_

#if defined(_WIN32) || defined(_WIN64)
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif // NOMINMAX
//  If _USING_V110_SDK71_ is defined it means we are using the v110_xp or v120_xp toolset.
#	if defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#		include <winapifamily.h>
#	endif // defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#	if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#		undef  GX_PLATFORM_WINDOWS
#		if !defined(WINVER) && !defined(_WIN32_WINNT)
#			if GX_ARCH_64BIT
//				When building 64-bit target Win7 and above.
#				define WINVER 0x0601
#				define _WIN32_WINNT 0x0601
#			else
//				Windows Server 2003 with SP1, Windows XP with SP2 and above
#				define WINVER 0x0502
#				define _WIN32_WINNT 0x0502
#			endif // GX_ARCH_64BIT
#		endif // !defined(WINVER) && !defined(_WIN32_WINNT)
#		define GX_PLATFORM_WINDOWS _WIN32_WINNT
#	elif defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)
#		undef  GX_PLATFORM_UWP
#		define GX_PLATFORM_UWP 1
#	elif defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_GAMES)
#		undef  GX_PLATFORM_GDK
#		define GX_PLATFORM_GDK 1
#	endif
#elif defined(__ANDROID__)
// Android compiler defines __linux__
#	include <sys/cdefs.h> // Defines __BIONIC__ and includes android/api-level.h
#	undef  GX_PLATFORM_ANDROID
#	define GX_PLATFORM_ANDROID __ANDROID_API__
#elif defined(__STEAMLINK__)
// SteamLink compiler defines __linux__
#	undef  GX_PLATFORM_STEAMLINK
#	define GX_PLATFORM_STEAMLINK 1
#elif defined(__VCCOREVER__)
// RaspberryPi compiler defines __linux__
#	undef  GX_PLATFORM_RPI
#	define GX_PLATFORM_RPI 1
#elif  defined(__linux__)
#	undef  GX_PLATFORM_LINUX
#	define GX_PLATFORM_LINUX 1
#elif  defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) \
 || defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
#	undef  GX_PLATFORM_IOS
#	define GX_PLATFORM_IOS 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#	undef  GX_PLATFORM_OSX
#	define GX_PLATFORM_OSX __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#elif defined(__EMSCRIPTEN__)
#	undef  GX_PLATFORM_EMSCRIPTEN
#	define GX_PLATFORM_EMSCRIPTEN 1
#elif defined(__ORBIS__)
#	undef  GX_PLATFORM_PS4
#	define GX_PLATFORM_PS4 1
#elif  defined(__FreeBSD__)        \
    || defined(__FreeBSD_kernel__) \
    || defined(__NetBSD__)         \
    || defined(__OpenBSD__)        \
    || defined(__DragonFly__)
#	undef  GX_PLATFORM_BSD
#	define GX_PLATFORM_BSD 1
#elif defined(__GNU__)
#	undef  GX_PLATFORM_HURD
#	define GX_PLATFORM_HURD 1
#elif defined(__NX__)
#	undef  GX_PLATFORM_NX
#	define GX_PLATFORM_NX 1
#endif //

#if !GX_CRT_NONE
// https://sourceforge.net/p/predef/wiki/Libraries/
#	if defined(__BIONIC__)
#		undef  GX_CRT_BIONIC
#		define GX_CRT_BIONIC 1
#	elif defined(_MSC_VER)
#		undef  GX_CRT_MSVC
#		define GX_CRT_MSVC 1
#	elif defined(__GLIBC__)
#		undef  GX_CRT_GLIBC
#		define GX_CRT_GLIBC (__GLIBC__ * 10000 + __GLIBC_MINOR__ * 100)
#	elif defined(__MINGW32__) || defined(__MINGW64__)
#		undef  GX_CRT_MINGW
#		define GX_CRT_MINGW 1
#	elif defined(__apple_build_version__) || defined(__ORBIS__) || defined(__EMSCRIPTEN__) || defined(__llvm__)
#		undef  GX_CRT_LIBCXX
#		define GX_CRT_LIBCXX 1
#	endif //

#	if !GX_CRT_BIONIC \
 && !GX_CRT_GLIBC  \
 && !GX_CRT_LIBCXX \
 && !GX_CRT_MINGW  \
 && !GX_CRT_MSVC   \
 && !GX_CRT_MUSL   \
 && !GX_CRT_NEWLIB
#		undef  GX_CRT_NONE
#		define GX_CRT_NONE 1
#	endif // GX_CRT_*
#endif // !GX_CRT_NONE

///
#define GX_PLATFORM_POSIX (0   \
    ||  GX_PLATFORM_ANDROID    \
    ||  GX_PLATFORM_BSD        \
    ||  GX_PLATFORM_EMSCRIPTEN \
    ||  GX_PLATFORM_HURD       \
    ||  GX_PLATFORM_IOS        \
    ||  GX_PLATFORM_LINUX      \
    ||  GX_PLATFORM_NX         \
    ||  GX_PLATFORM_OSX        \
    ||  GX_PLATFORM_PS4        \
    ||  GX_PLATFORM_RPI        \
    ||  GX_PLATFORM_STEAMLINK  \
    )

///
#define GX_PLATFORM_NONE !(0   \
    ||  GX_PLATFORM_ANDROID    \
    ||  GX_PLATFORM_BSD        \
    ||  GX_PLATFORM_EMSCRIPTEN \
    ||  GX_PLATFORM_HURD       \
    ||  GX_PLATFORM_IOS        \
    ||  GX_PLATFORM_LINUX      \
    ||  GX_PLATFORM_NX         \
    ||  GX_PLATFORM_OSX        \
    ||  GX_PLATFORM_PS4        \
    ||  GX_PLATFORM_RPI        \
    ||  GX_PLATFORM_STEAMLINK  \
    ||  GX_PLATFORM_WINDOWS    \
    ||  GX_PLATFORM_UWP        \
    ||  GX_PLATFORM_GDK        \
    )

///
#define GX_PLATFORM_OS_CONSOLE  (0 \
    ||  GX_PLATFORM_NX             \
    ||  GX_PLATFORM_PS4            \
    ||  GX_PLATFORM_UWP            \
    ||  GX_PLATFORM_GDK            \
    )

///
#define GX_PLATFORM_OS_DESKTOP  (0 \
    ||  GX_PLATFORM_BSD            \
    ||  GX_PLATFORM_HURD           \
    ||  GX_PLATFORM_LINUX          \
    ||  GX_PLATFORM_OSX            \
    ||  GX_PLATFORM_WINDOWS        \
    )

///
#define GX_PLATFORM_OS_EMBEDDED (0 \
    ||  GX_PLATFORM_RPI            \
    ||  GX_PLATFORM_STEAMLINK      \
    )

///
#define GX_PLATFORM_OS_MOBILE   (0 \
    ||  GX_PLATFORM_ANDROID        \
    ||  GX_PLATFORM_IOS            \
    )

///
#define GX_PLATFORM_OS_WEB      (0 \
    ||  GX_PLATFORM_EMSCRIPTEN     \
    )

///
#if GX_COMPILER_GCC
#	define GX_COMPILER_NAME "GCC "       \
        GX_STRINGIZE(__GNUC__) "."       \
        GX_STRINGIZE(__GNUC_MINOR__) "." \
        GX_STRINGIZE(__GNUC_PATCHLEVEL__)
#elif GX_COMPILER_CLANG
#	define GX_COMPILER_NAME "Clang "      \
        GX_STRINGIZE(__clang_major__) "." \
        GX_STRINGIZE(__clang_minor__) "." \
        GX_STRINGIZE(__clang_patchlevel__)
#elif GX_COMPILER_MSVC
#	if GX_COMPILER_MSVC >= 1910 // Visual Studio 2017
#		define GX_COMPILER_NAME "MSVC 15.0"
#	elif GX_COMPILER_MSVC >= 1900 // Visual Studio 2015
#		define GX_COMPILER_NAME "MSVC 14.0"
#	elif GX_COMPILER_MSVC >= 1800 // Visual Studio 2013
#		define GX_COMPILER_NAME "MSVC 12.0"
#	elif GX_COMPILER_MSVC >= 1700 // Visual Studio 2012
#		define GX_COMPILER_NAME "MSVC 11.0"
#	elif GX_COMPILER_MSVC >= 1600 // Visual Studio 2010
#		define GX_COMPILER_NAME "MSVC 10.0"
#	elif GX_COMPILER_MSVC >= 1500 // Visual Studio 2008
#		define GX_COMPILER_NAME "MSVC 9.0"
#	else
#		define GX_COMPILER_NAME "MSVC"
#	endif //
#endif // GX_COMPILER_

#if GX_PLATFORM_ANDROID
#	define GX_PLATFORM_NAME "Android " \
                GX_STRINGIZE(GX_PLATFORM_ANDROID)
#elif GX_PLATFORM_BSD
#	define GX_PLATFORM_NAME "BSD"
#elif GX_PLATFORM_EMSCRIPTEN
#	define GX_PLATFORM_NAME "asm.js "          \
        GX_STRINGIZE(__EMSCRIPTEN_major__) "." \
        GX_STRINGIZE(__EMSCRIPTEN_minor__) "." \
        GX_STRINGIZE(__EMSCRIPTEN_tiny__)
#elif GX_PLATFORM_HURD
#	define GX_PLATFORM_NAME "Hurd"
#elif GX_PLATFORM_IOS
#	define GX_PLATFORM_NAME "iOS"
#elif GX_PLATFORM_LINUX
#	define GX_PLATFORM_NAME "Linux"
#elif GX_PLATFORM_NONE
#	define GX_PLATFORM_NAME "None"
#elif GX_PLATFORM_NX
#	define GX_PLATFORM_NAME "NX"
#elif GX_PLATFORM_OSX
#	define GX_PLATFORM_NAME "OSX"
#elif GX_PLATFORM_PS4
#	define GX_PLATFORM_NAME "PlayStation 4"
#elif GX_PLATFORM_RPI
#	define GX_PLATFORM_NAME "RaspberryPi"
#elif GX_PLATFORM_STEAMLINK
#	define GX_PLATFORM_NAME "SteamLink"
#elif GX_PLATFORM_WINDOWS
#	define GX_PLATFORM_NAME "Windows"
#elif GX_PLATFORM_UWP
#	define GX_PLATFORM_NAME "WindowsStore"
#elif GX_PLATFORM_GDK
#	define GX_PLATFORM_NAME "Xbox"
#else
#	error "Unknown GX_PLATFORM!"
#endif // GX_PLATFORM_

#if GX_CPU_ARM
#	define GX_CPU_NAME "ARM"
#elif GX_CPU_JIT
#	define GX_CPU_NAME "JIT-VM"
#elif GX_CPU_MIPS
#	define GX_CPU_NAME "MIPS"
#elif GX_CPU_PPC
#	define GX_CPU_NAME "PowerPC"
#elif GX_CPU_RISCV
#	define GX_CPU_NAME "RISC-V"
#elif GX_CPU_X86
#	define GX_CPU_NAME "x86"
#endif // GX_CPU_

#if GX_CRT_BIONIC
#	define GX_CRT_NAME "Bionic libc"
#elif GX_CRT_GLIBC
#	define GX_CRT_NAME "GNU C Library"
#elif GX_CRT_MSVC
#	define GX_CRT_NAME "MSVC C Runtime"
#elif GX_CRT_MINGW
#	define GX_CRT_NAME "MinGW C Runtime"
#elif GX_CRT_LIBCXX
#	define GX_CRT_NAME "Clang C Library"
#elif GX_CRT_NEWLIB
#	define GX_CRT_NAME "Newlib"
#elif GX_CRT_MUSL
#	define GX_CRT_NAME "musl libc"
#elif GX_CRT_NONE
#	define GX_CRT_NAME "None"
#else
#	error "Unknown GX_CRT!"
#endif // GX_CRT_

#if GX_ARCH_32BIT
#	define GX_ARCH_NAME "32-bit"
#elif GX_ARCH_64BIT
#	define GX_ARCH_NAME "64-bit"
#endif // GX_ARCH_

#if GX_COMPILER_MSVC
#	define GX_CPP_NAME "C++MsvcUnknown"
#elif defined(__cplusplus)
#	if __cplusplus < 201103L
#		error "Pre-C++11 compiler is not supported!"
#	elif __cplusplus < 201402L
#		define GX_CPP_NAME "C++11"
#	elif __cplusplus < 201703L
#		define GX_CPP_NAME "C++14"
#	elif __cplusplus < 201704L
#		define GX_CPP_NAME "C++17"
#	else
// See: https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b#orthodox-c
#		define GX_CPP_NAME "C++WayTooModern"
#	endif // GX_CPP_NAME
#else
#	define GX_CPP_NAME "C++Unknown"
#endif // defined(__cplusplus)

#endif //GX_PLATFORM_H
