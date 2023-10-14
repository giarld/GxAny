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

#define IS_GANY_CORE 1

#include "gx/gany_core.h"
#include "gx/gany.h"

#include "gany_env_object.h"
#include "gany_iterator.h"

#include "ref_gstring.h"

#include <utility>
#include <sys/stat.h>
#include <fstream>

#if GX_PLATFORM_WINDOWS

#include <windows.h>

#else

#include <dirent.h>
#include <dlfcn.h>

#endif

#define MAX_ABS_PATH 2048


typedef void (*RegisterModuleFunc)(PFN_ganyGetEnv pfnGetEnv, PFN_ganyParseJson pfnParseJson,
                                   PFN_ganyRegisterToEnv pfnRegisterToEnv, PFN_ganyClassInstance pfnClassInstance);

GX_NS_BEGIN

void *dlOpen(const std::string &path)
{
#if GX_PLATFORM_WINDOWS
    GWString wStr = GString(path).toUtf16();
    return (void *) ::LoadLibraryW(wStr.data());
#else
    return ::dlopen(path.c_str(), RTLD_LOCAL | RTLD_LAZY);
#endif
}

void *dlSym(void *handle, const std::string &symbol)
{
#if GX_PLATFORM_WINDOWS
    return (void *) ::GetProcAddress((HMODULE) handle, symbol.c_str());
#else
    return ::dlsym(handle, symbol.c_str());
#endif
}

bool fileExists(const GString &path)
{
    if (path.isEmpty()) {
        return false;
    }
#if GX_PLATFORM_WINDOWS
    return _waccess(path.toUtf16().data(), 0) == 0;
#else
    struct stat fstat{};
    return stat(path.c_str(), &fstat) == 0;
#endif
}

GString formatPath(GString path)
{
    if (path.isEmpty()) {
        return "";
    }
    path = path.replace("\\", "/");
    if (path.endWith("/")) {
        path = path.left(path.length() - 1);
#if GX_PLATFORM_WINDOWS || GX_PLATFORM_WINRT
        if (path.endWith(":")) {
            path += "/";
        }
#else
        if (path.isEmpty()) {
            path += "/";
        }
#endif
    }
#if GX_PLATFORM_WINDOWS || GX_PLATFORM_WINRT
    if (path.length() == 2 && path.endWith(":")) {
        path += "/";
    }
#endif
    return path;
}

GString absoluteFilePath(const GString &path)
{
    if (path.isEmpty()) {
        return "";
    }
    GString absString;
#if GX_PLATFORM_WINDOWS || GX_PLATFORM_WINRT
    wchar_t wAbsPath[MAX_ABS_PATH];
    _wfullpath(wAbsPath, path.toUtf16().data(), MAX_ABS_PATH);
    absString = GString(GWString(wAbsPath));
#elif GX_PLATFORM_POSIX
    char absPath[PATH_MAX];
    if (realpath(path.c_str(), absPath)) {
        absString = absPath;
    } else {
        absString = path;
    }
#else
    #error "Unsupported platform!!"
#endif
    return formatPath(absString);
}


static GAny &loadPluginFuncs()
{
    static GAny sLoadPluginFuncs = GAny::object();
    return sLoadPluginFuncs;
}

std::vector<std::string> &pluginSearchPaths()
{
    static std::vector<std::string> sPluginSearchPaths;
    return sPluginSearchPaths;
}

void setPluginLoaders(const std::string &pluginType,
                      const std::function<bool(const std::string &, const std::string &)> &func)
{
    loadPluginFuncs().setItem(pluginType, func);
}

/// ================ Tool function ================

void setPluginSearchPath(const std::string &path)
{
    if (!fileExists(path)) {
        return;
    }

    std::string absPath = absoluteFilePath(path).toStdString();

    auto &searchPaths = pluginSearchPaths();

    auto fIt = std::find(searchPaths.begin(), searchPaths.end(), absPath);
    if (fIt != searchPaths.end()) {
        return;
    }
    searchPaths.push_back(absPath);
}

const std::vector<std::string> &getPluginSearchPaths()
{
    return pluginSearchPaths();
}

bool loadCPlugin(const std::string &searchPath, const std::string &pluginName)
{
    GString pluginNameStr = pluginName;
    int64_t lastSplitIndex = pluginNameStr.lastIndexOf("/");
    if (lastSplitIndex < 0) {
        std::cerr << "Invalid C/C++ plugin naming." << std::endl;
        return false;
    }
    std::string libName = pluginNameStr.substring(0, lastSplitIndex).toStdString();
    std::string moduleName = pluginNameStr.substring(lastSplitIndex + 1).toStdString();

    GString dir = absoluteFilePath(searchPath);
    GString libFile = dir + "/" + libName;
    while (!fileExists(libFile)) {
#if GX_PLATFORM_WINDOWS
        libFile = dir + "/" + libName + ".dll";
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir + "/lib" + libName + ".dll";
#else
#if GX_PLATFORM_LINUX || GX_PLATFORM_ANDROID || GX_PLATFORM_BSD
        libFile = dir + "/lib" + libName + ".so";
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir + "/" + libName + ".so";
#elif GX_PLATFORM_OSX || GX_PLATFORM_IOS
        libFile = dir + "/lib" + libName + ".dylib";
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir + "/lib" + libName + ".bundle";
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir + "/lib" + libName + ".so";
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir + "/" + libName + ".dylib";
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir + "/" + libName + ".bundle";
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir + "/" + libName + ".so";
#endif
#endif
        break;
    }

    if (fileExists(libFile)) {
        void *lib = dlOpen(libFile.c_str());
        void *regFuncPtr = dlSym(lib, "Register" + moduleName);
        if (!regFuncPtr) {
            return false;
        }
        auto regFunc = (RegisterModuleFunc) regFuncPtr;
        regFunc(pfnGanyGetEnv, pfnGanyParseJson, pfnGanyRegisterToEnv, pfnGanyClassInstance);
        return true;
    }
    return false;
}

bool _loadPlugin(const std::string &searchPath, const std::string &pluginName)
{
    if (loadCPlugin(searchPath, pluginName)) {
        return true;
    }
    for (auto it = loadPluginFuncs().iterator(); it.hasNext();) {
        auto item = it.next();
        auto loader = item.second;
        if (loader.isFunction() && loader(searchPath, pluginName).toBool()) {
#if GX_DEBUG
            std::cout << "Loaded " << item.first.toString() << " plugin: " << pluginName << std::endl;
#endif
            return true;
        }
    }

    return false;
}

bool loadPlugin(const std::string &searchPath, const std::string &pluginName)
{
    if (!fileExists(searchPath)) {
        std::cerr << "loadPlugin, plugin dir " << searchPath << " not exists." << std::endl;
        return false;
    }
    if (_loadPlugin(searchPath, pluginName)) {
        return true;
    }

    std::cerr << "loadPlugin, plugin " << pluginName << " load failed." << std::endl;
    return false;
}

bool loadPlugin(const std::string &pluginName)
{
    const auto &searchPaths = getPluginSearchPaths();
    for (auto it = searchPaths.rbegin(); it != searchPaths.rend(); it++) {
        if (_loadPlugin(*it, pluginName)) {
            return true;
        }
    }
    std::cerr << "loadPlugin, plugin " << pluginName << " load failed." << std::endl;
    return false;
}

static void initPluginLoader()
{
    setPluginSearchPath("./");  // The search path is a reversed list, with higher priority given to items added later

    GAny loadPluginFunc = [](const std::string &searchPath, const std::string &pluginName) {
        return loadPlugin(searchPath, pluginName);
    };
    loadPluginFunc.overload([](const std::string &pluginName) {
        return loadPlugin(pluginName);
    });

    getEnvObject().set("loadPlugin", loadPluginFunc);
    getEnvObject().set("setPluginSearchPath", &setPluginSearchPath);
    getEnvObject().set("getPluginSearchPaths", &getPluginSearchPaths);
    getEnvObject().set("setPluginLoaders", [](const std::string &pluginType, const GAny &loaderFunc) {
        if (!loaderFunc.isFunction()) {
            return ;
        }
        loadPluginFuncs().setItem(pluginType, loaderFunc);
    });
}

GX_NS_END

REGISTER_GANY_MODULE(Builtin)
{
    using namespace gx;
    /// int32
    GAnyClass::Class < int32_t > ()
            ->setName("int32")
            .setDoc("int32_t")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<int32_t>()) {
                    return rh.as<int32_t>();
                }
                return rh.toInt32();
            })
            .func(MetaFunction::ToObject, [](int32_t &i) { return i; })
            .func(MetaFunction::EqualTo, [](int32_t &self, int32_t rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](int32_t &self, int32_t rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](int32_t &self, const GAny &rh) -> GAny {
                     return self + rh.toInt32();
                 })
            .func(MetaFunction::Subtraction,
                 [](int32_t self, const GAny &rh) -> GAny {
                     return self - rh.toInt32();
                 })
            .func(MetaFunction::Multiplication,
                 [](int32_t &self, const GAny &rh) -> GAny {
                     return self * rh.toInt32();
                 })
            .func(MetaFunction::Division,
                 [](int32_t &self, const GAny &rh) -> GAny {
                     return self / rh.toInt32();
                 })
            .func(MetaFunction::Modulo,
                 [](int32_t &self, int32_t rh) {
                     return self % rh;
                 })
            .func(MetaFunction::Negate, [](int32_t &self) { return (int32_t) (-self); })
            .func(MetaFunction::BitXor, [](int32_t &self, int32_t &rh) { return (int32_t) (self ^ rh); })
            .func(MetaFunction::BitOr, [](int32_t &self, int32_t &rh) { return (int32_t) (self | rh); })
            .func(MetaFunction::BitAnd, [](int32_t &self, int32_t &rh) { return (int32_t) (self & rh); })
            .func(MetaFunction::BitNot, [](int32_t &self) { return (int32_t) ~self; });

    /// uint32_t
    GAnyClass::Class < uint32_t > ()
            ->setName("uint32")
            .setDoc("uint32_t")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<uint32_t>()) {
                    return rh.as<uint32_t>();
                }
                return (uint32_t) rh.toInt32();
            })
            .func(MetaFunction::ToObject, [](uint32_t &i) { return i; })
            .func(MetaFunction::EqualTo, [](uint32_t &self, uint32_t rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](uint32_t &self, uint32_t rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](uint32_t &self, const GAny &rh) -> GAny {
                     return self + (uint32_t) rh.toInt32();
                 })
            .func(MetaFunction::Subtraction,
                 [](uint32_t self, const GAny &rh) -> GAny {
                     return self - (uint32_t) rh.toInt32();
                 })
            .func(MetaFunction::Multiplication,
                 [](uint32_t &self, const GAny &rh) -> GAny {
                     return self * (uint32_t) rh.toInt32();
                 })
            .func(MetaFunction::Division,
                 [](uint32_t &self, const GAny &rh) -> GAny {
                     return self / (uint32_t) rh.toInt32();
                 })
            .func(MetaFunction::Modulo,
                 [](uint32_t &self, uint32_t rh) {
                     return self % rh;
                 })
            .func(MetaFunction::Negate, [](uint32_t &self) { return (int32_t) (-self); })
            .func(MetaFunction::BitXor, [](uint32_t &self, uint32_t &rh) { return (uint32_t) (self ^ rh); })
            .func(MetaFunction::BitOr, [](uint32_t &self, uint32_t &rh) { return (uint32_t) self | rh; })
            .func(MetaFunction::BitAnd, [](uint32_t &self, uint32_t &rh) { return (uint32_t) (self & rh); })
            .func(MetaFunction::BitNot, [](uint32_t &self) { return (uint32_t) ~self; });

    /// int64_t
    GAnyClass::Class < int64_t > ()
            ->setName("int64")
            .setDoc("int64_t")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<int64_t>()) {
                    return rh.as<int64_t>();
                }
                return rh.toInt64();
            })
            .func(MetaFunction::ToObject, [](int64_t &i) { return i; })
            .func(MetaFunction::EqualTo, [](int64_t &self, int64_t rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](int64_t &self, int64_t rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](int64_t &self, const GAny &rh) -> GAny {
                     return self + rh.toInt64();
                 })
            .func(MetaFunction::Subtraction,
                 [](int64_t self, const GAny &rh) -> GAny {
                     return self - rh.toInt64();
                 })
            .func(MetaFunction::Multiplication,
                 [](int64_t &self, const GAny &rh) -> GAny {
                     return self * rh.toInt64();
                 })
            .func(MetaFunction::Division,
                 [](int64_t &self, const GAny &rh) -> GAny {
                     return self / rh.toInt64();
                 })
            .func(MetaFunction::Modulo,
                 [](int64_t &self, int64_t rh) {
                     return self % rh;
                 })
            .func(MetaFunction::Negate, [](int64_t &self) { return (int64_t) (-self); })
            .func(MetaFunction::BitXor, [](int64_t &self, int64_t &rh) { return (int64_t) (self ^ rh); })
            .func(MetaFunction::BitOr, [](int64_t &self, int64_t &rh) { return (int64_t) (self | rh); })
            .func(MetaFunction::BitAnd, [](int64_t &self, int64_t &rh) { return (int64_t) (self & rh); })
            .func(MetaFunction::BitNot, [](int64_t &self) { return (int64_t) ~self; });

    /// uint64_t
    GAnyClass::Class < uint64_t > ()
            ->setName("uint64")
            .setDoc("uint64_t")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<uint64_t>()) {
                    return rh.as<uint64_t>();
                }
                return (uint64_t) rh.toInt64();
            })
            .func(MetaFunction::ToObject, [](uint64_t &i) { return i; })
            .func(MetaFunction::EqualTo, [](uint64_t &self, uint64_t rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](uint64_t &self, uint64_t rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](uint64_t &self, const GAny &rh) -> GAny {
                     return self + (uint64_t) rh.toInt64();
                 })
            .func(MetaFunction::Subtraction,
                 [](uint64_t self, const GAny &rh) -> GAny {
                     return self - (uint64_t) rh.toInt64();
                 })
            .func(MetaFunction::Multiplication,
                 [](uint64_t &self, const GAny &rh) -> GAny {
                     return self * (uint64_t) rh.toInt64();
                 })
            .func(MetaFunction::Division,
                 [](uint64_t &self, const GAny &rh) -> GAny {
                     return self / (uint64_t) rh.toInt64();
                 })
            .func(MetaFunction::Modulo,
                 [](uint64_t &self, uint64_t rh) {
                     return self % rh;
                 })
            .func(MetaFunction::Negate, [](uint64_t &self) { return (int64_t) (-self); })
            .func(MetaFunction::BitXor, [](uint64_t &self, uint64_t &rh) { return (uint64_t) (self ^ rh); })
            .func(MetaFunction::BitOr, [](uint64_t &self, uint64_t &rh) { return (uint64_t) (self | rh); })
            .func(MetaFunction::BitAnd, [](uint64_t &self, uint64_t &rh) { return (uint64_t) (self & rh); })
            .func(MetaFunction::BitNot, [](uint64_t &self) { return (uint64_t) ~self; });

    /// float
    GAnyClass::Class < float > ()
            ->setName("float")
            .setDoc("float")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<float>()) {
                    return rh.as<float>();
                }
                return (float) rh.toDouble();
            })
            .func(MetaFunction::ToObject, [](float &i) { return i; })
            .func(MetaFunction::EqualTo, [](float &self, float rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](float &self, float rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](float &self, const GAny &rh) -> GAny {
                     return self + rh.toFloat();
                 })
            .func(MetaFunction::Subtraction,
                 [](float self, const GAny &rh) -> GAny {
                     return self - rh.toFloat();
                 })
            .func(MetaFunction::Multiplication,
                 [](float &self, const GAny &rh) -> GAny {
                     return self * rh.toFloat();
                 })
            .func(MetaFunction::Division,
                 [](float &self, const GAny &rh) {
                     return self / rh.toFloat();
                 })
            .func(MetaFunction::Negate, [](float &self) { return -self; });

    /// double
    GAnyClass::Class < double > ()
            ->setName("double")
            .setDoc("double")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<double>()) {
                    return rh.as<double>();
                }
                return (double) rh.toDouble();
            })
            .func(MetaFunction::ToObject, [](double &i) { return i; })
            .func(MetaFunction::EqualTo, [](double &self, double rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](double &self, double rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](double &self, const GAny &rh) -> GAny {
                     return self + rh.toDouble();
                 })
            .func(MetaFunction::Subtraction,
                 [](double self, const GAny &rh) -> GAny {
                     return self - rh.toDouble();
                 })
            .func(MetaFunction::Multiplication,
                 [](double &self, const GAny &rh) -> GAny {
                     return self * rh.toDouble();
                 })
            .func(MetaFunction::Division,
                 [](double &self, const GAny &rh) {
                     return self / rh.toDouble();
                 })
            .func(MetaFunction::Negate, [](double &self) { return -self; });

    /// bool
    GAnyClass::Class < bool > ()
            ->setName("bool")
            .setDoc("boolean")
            .func(MetaFunction::Init, [](const GAny &rh) {
                return rh.toBool();
            })
            .func(MetaFunction::ToObject, [](bool &i) { return i; })
            .func(MetaFunction::EqualTo, [](bool &self, bool &rh) { return (bool) (self == rh); });

    /// int16_t
    GAnyClass::Class < int16_t > ()
            ->setName("int16")
            .setDoc("int16_t")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<int16_t>()) {
                    return rh.as<int16_t>();
                }
                return rh.toInt16();
            })
            .func(MetaFunction::ToObject, [](int16_t &i) { return i; })
            .func(MetaFunction::EqualTo, [](int16_t &self, int16_t &rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](int16_t &self, int16_t rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](int16_t &self, const GAny &rh) -> GAny {
                     return self + rh.toInt16();
                 })
            .func(MetaFunction::Subtraction,
                 [](int16_t self, const GAny &rh) -> GAny {
                     return self - rh.toInt16();
                 })
            .func(MetaFunction::Multiplication,
                 [](int16_t &self, const GAny &rh) -> GAny {
                     return self * rh.toInt16();
                 })
            .func(MetaFunction::Division,
                 [](int16_t &self, const GAny &rh) -> GAny {
                     return self / rh.toInt16();
                 })
            .func(MetaFunction::Modulo,
                 [](int16_t &self, int16_t rh) {
                     return self % rh;
                 })
            .func(MetaFunction::Negate, [](int16_t &self) { return (int16_t) (-self); })
            .func(MetaFunction::BitXor, [](int16_t &self, int16_t &rh) { return (int16_t) (self ^ rh); })
            .func(MetaFunction::BitOr, [](int16_t &self, int16_t &rh) { return (int16_t) (self | rh); })
            .func(MetaFunction::BitAnd, [](int16_t &self, int16_t &rh) { return (int16_t) (self & rh); })
            .func(MetaFunction::BitNot, [](int16_t &self) { return (int16_t) ~self; });

    /// uint16_t
    GAnyClass::Class < uint16_t > ()
            ->setName("uint16")
            .setDoc("uint16_t")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<uint16_t>()) {
                    return rh.as<uint16_t>();
                }
                return (uint16_t) rh.toInt16();
            })
            .func(MetaFunction::ToObject, [](uint16_t &i) { return i; })
            .func(MetaFunction::EqualTo, [](uint16_t &self, uint16_t &rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](uint16_t &self, uint16_t rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](uint16_t &self, const GAny &rh) -> GAny {
                     return self + (uint16_t) rh.toInt16();
                 })
            .func(MetaFunction::Subtraction,
                 [](uint16_t self, const GAny &rh) -> GAny {
                     return self - (uint16_t) rh.toInt16();
                 })
            .func(MetaFunction::Multiplication,
                 [](uint16_t &self, const GAny &rh) -> GAny {
                     return self * (uint16_t) rh.toInt16();
                 })
            .func(MetaFunction::Division,
                 [](uint16_t &self, const GAny &rh) -> GAny {
                     return self / (uint16_t) rh.toInt16();
                 })
            .func(MetaFunction::Modulo,
                 [](uint16_t &self, uint16_t rh) {
                     return self % rh;
                 })
            .func(MetaFunction::Negate, [](uint16_t &self) { return (int16_t) (-self); })
            .func(MetaFunction::BitXor, [](uint16_t &self, uint16_t &rh) { return (uint16_t) (self ^ rh); })
            .func(MetaFunction::BitOr, [](uint16_t &self, uint16_t &rh) { return (uint16_t) (self | rh); })
            .func(MetaFunction::BitAnd, [](uint16_t &self, uint16_t &rh) { return (uint16_t) (self & rh); })
            .func(MetaFunction::BitNot, [](uint16_t &self) { return (uint16_t) ~self; });

    /// int8_t
    GAnyClass::Class < int8_t > ()
            ->setName("int8")
            .setDoc("int8_t")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<int8_t>()) {
                    return rh.as<int8_t>();
                }
                return rh.toInt8();
            })
            .func(MetaFunction::ToObject, [](int8_t &i) { return i; })
            .func(MetaFunction::EqualTo, [](int8_t &self, int8_t rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](int8_t &self, int8_t rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](int8_t &self, const GAny &rh) -> GAny {
                     return self + rh.toInt8();
                 })
            .func(MetaFunction::Subtraction,
                 [](int8_t self, const GAny &rh) -> GAny {
                     return self - rh.toInt32();
                 })
            .func(MetaFunction::Multiplication,
                 [](int8_t &self, const GAny &rh) -> GAny {
                     return self * rh.toInt32();
                 })
            .func(MetaFunction::Division,
                 [](int8_t &self, const GAny &rh) -> GAny {
                     return self / rh.toInt32();
                 })
            .func(MetaFunction::Modulo,
                 [](int8_t &self, int8_t rh) {
                     return self % rh;
                 })
            .func(MetaFunction::Negate, [](int8_t &self) { return (int8_t) (-self); })
            .func(MetaFunction::BitXor, [](int8_t &self, int8_t &rh) { return (int8_t) (self ^ rh); })
            .func(MetaFunction::BitOr, [](int8_t &self, int8_t &rh) { return (int8_t) (self | rh); })
            .func(MetaFunction::BitAnd, [](int8_t &self, int8_t &rh) { return (int8_t) (self & rh); })
            .func(MetaFunction::BitNot, [](int8_t &self) { return (int8_t) ~self; });

    /// uint8_t
    GAnyClass::Class < uint8_t > ()
            ->setName("uint8")
            .setDoc("uint8_t")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<uint8_t>()) {
                    return rh.as<uint8_t>();
                }
                return (uint8_t) rh.toInt8();
            })
            .func(MetaFunction::ToObject, [](uint8_t &i) { return i; })
            .func(MetaFunction::EqualTo, [](uint8_t &self, uint8_t rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](uint8_t &self, uint8_t rh) { return self < rh; })
            .func(MetaFunction::Addition,
                 [](uint8_t &self, const GAny &rh) -> GAny {
                     return self + (uint8_t) rh.toInt32();
                 })
            .func(MetaFunction::Subtraction,
                 [](uint8_t self, const GAny &rh) -> GAny {
                     return self - (uint8_t) rh.toInt8();
                 })
            .func(MetaFunction::Multiplication,
                 [](uint8_t &self, const GAny &rh) -> GAny {
                     return self * (uint8_t) rh.toInt8();
                 })
            .func(MetaFunction::Division,
                 [](uint8_t &self, const GAny &rh) -> GAny {
                     return self / (uint8_t) rh.toInt8();
                 })
            .func(MetaFunction::Modulo,
                 [](uint8_t &self, uint8_t rh) {
                     return self % rh;
                 })
            .func(MetaFunction::Negate, [](uint8_t &self) { return (int8_t) (-self); })
            .func(MetaFunction::BitXor,
                 [](uint8_t &self, uint8_t &rh) { return (uint8_t) (self ^ rh); })
            .func(MetaFunction::BitOr,
                 [](uint8_t &self, uint8_t &rh) { return (uint8_t) (self | rh); })
            .func(MetaFunction::BitAnd,
                 [](uint8_t &self, uint8_t &rh) { return (uint8_t) (self & rh); })
            .func(MetaFunction::BitNot, [](uint8_t &self) { return (uint8_t) ~self; });

    /// string
    GAnyClass::Class < std::string > ()
            ->setName("string")
            .setDoc("string")
            .func(MetaFunction::Init, [](const GAny &rh) {
                if (rh.is<std::string>()) {
                    return rh.as<std::string>();
                }
                return rh.toString();
            })
            .func(MetaFunction::ToObject, [](std::string &s) { return s; })
            .func(MetaFunction::Length, [](const std::string &self) { return self.size(); })
            .func(MetaFunction::Addition,
                 [](const std::string &self, const std::string &rh) {
                     return self + rh;
                 })
            .func(MetaFunction::EqualTo, [](const std::string &self, std::string &rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](const std::string &self, std::string &rh) { return self < rh; })
            .func(MetaFunction::SetItem, [](std::string &self, int32_t index, char c) {
                if (index < 0 || index >= self.size()) {
                    return;
                }
                self[index] = c;
            })
            .func(MetaFunction::GetItem, [](std::string &self, int32_t index) {
                if (index < 0 || index >= self.size()) {
                    return '\0';
                }
                return self[index];
            })
            .func("c_str", [](std::string &self) { return self.c_str(); });

    GAny::null()
            .classObject()
            .func(MetaFunction::ToString, [](const GAny &) { return "null"; })
            .func(MetaFunction::EqualTo, [](const GAny &a, const GAny &b) {
                return a.isNull() && b.isNull();
            })
            .func(MetaFunction::LessThan, [](const GAny &a, const GAny &b) {
                return false;
            });

    GAnyClass::Class <
    const char *>()
            ->setName("const char *")
            .setDoc("const char *")
            .func(MetaFunction::ToString,
                 [](const char *str) {
                     return std::string(str);
                 })
            .func(MetaFunction::ToObject,
                 [](const char *str) {
                     return std::string(str);
                 })
            .func(MetaFunction::EqualTo,
                 [](const char *a, const char *b) {
                     return strcmp(a, b) == 0;
                 })
            .func(MetaFunction::LessThan,
                 [](const char *a, const char *b) {
                     return strcmp(a, b) < 0;
                 });

    GAnyClass::Class < GAnyArray > ()
            ->setName("GAnyArray")
            .setDoc("GAnyArray")
            .func(MetaFunction::Addition,
                 [](const GAnyArray &self, const GAnyArray &other) {
                     GLockerGuard locker1(self.lock);
                     GLockerGuard locker2(other.lock);
                     std::vector<GAny> ret(self.var);
                     ret.insert(ret.end(), other.var.begin(), other.var.end());
                     return ret;
                 })
            .func(MetaFunction::Multiplication,
                 [](const GAnyArray &self, const int &num) {
                     GLockerGuard locker1(self.lock);
                     std::vector<GAny> ret;
                     for (int i = 0; i < num; ++i) {
                         ret.insert(ret.end(), self.var.begin(), self.var.end());
                     }
                     return ret;
                 })
            .func(MetaFunction::EqualTo, [](const GAnyArray &self, const GAnyArray &rhs) {
                if (self.length() != rhs.length()) {
                    return false;
                }

                try {
                    auto size = (int32_t) self.length();
                    for (int32_t i = 0; i < size; i++) {
                        if (self[i] != rhs[i]) {
                            return false;
                        }
                    }
                } catch (GAnyException &e) {
                    std::cerr << "GAnyArray::EqualTo Exception: " << e.what() << std::endl;
                    return false;
                }

                return true;
            })
            .func("forEach", [](GAnyArray &self, const GAny &func) {
                // func: function(const GAny &value)->bool
                GLockerGuard locker(self.lock);
                for (auto &item: self.var) {
                    auto ret = func(item);
                    if (ret.isBoolean() && !ret.toBool()) {
                        break;
                    }
                }
            })
            .func("insert", [](GAnyArray &self, int32_t index, const GAny &val) {
                GLockerGuard locker(self.lock);
                if (index < 0 || index > self.var.size()) {
                    return;
                }
                auto it = self.var.begin() + index;
                self.var.insert(it, val);
            })
            .func("sort", [](GAnyArray &self, const GAny &comp) {
                GLockerGuard locker(self.lock);
                if (comp.isFunction()) {
                    std::sort(self.var.begin(), self.var.end(), [&comp](const GAny &lhs, const GAny &rhs) {
                        return comp(lhs, rhs).toBool();
                    });
                }
            })
            .func("iterator", [](GAnyArray &self) {
                return std::make_unique<GAnyArrayIterator>(self.var);
            });

    GAnyClass::Class < GAnyObject > ()
            ->setName("GAnyObject")
            .setDoc("GAnyObject")
            .func(MetaFunction::Addition,
                 [](const GAnyObject &self, const GAnyObject &rhs) {
                     if (&self == &rhs) {
                         GLockerGuard locker1(self.lock);
                         return self.var;
                     } else {
                         auto ret = self.var;
                         GLockerGuard locker1(self.lock);
                         GLockerGuard locker2(rhs.lock);
                         for (const auto &it: rhs.var) {
                             if (ret.find(it.first) == ret.end()) {
                                 ret.insert(it);
                             }
                         }
                         return ret;
                     }
                 })
            .func(MetaFunction::EqualTo, [](const GAnyObject &self, const GAnyObject &rhs) {
                if (self.length() != rhs.length()) {
                    return false;
                }

                try {
                    GLockerGuard locker1(self.lock);
                    for (const auto &it: self.var) {
                        std::string k = it.first;
                        const auto &v = it.second;
                        if (!rhs.contains(k)) {
                            return false;
                        }
                        if (v != rhs[k]) {
                            return false;
                        }
                    }
                } catch (GAnyException &e) {
                    std::cerr << "GAnyObject::EqualTo Exception: " << e.what() << std::endl;
                    return false;
                }

                return true;
            })
            .func("forEach", [](GAnyObject &self, const GAny &func) {
                // func: function(const std::string &key, const GAny &value)->bool
                GLockerGuard locker(self.lock);
                for (auto &item: self.var) {
                    auto ret = func(item.first, item.second);
                    if (ret.isBoolean() && !ret.toBool()) {
                        break;
                    }
                }
            })
            .func("iterator", [](GAnyObject &self) {
                return std::make_unique<GAnyObjectIterator>(self.var);
            });

    GAnyClass::Class < GAnyArrayIterator > ()
            ->setName("GAnyArrayIterator")
            .setDoc("GAnyArray iterator.")
            .func("hasNext", &GAnyArrayIterator::hasNext)
            .func("next", &GAnyArrayIterator::next)
            .func("remove", &GAnyArrayIterator::remove)
            .func("hasPrevious", &GAnyArrayIterator::hasPrevious)
            .func("previous", &GAnyArrayIterator::previous)
            .func("toFront", &GAnyArrayIterator::toFront)
            .func("toBack", &GAnyArrayIterator::toBack);

    GAnyClass::Class < GAnyObjectIterator > ()
            ->setName("GAnyObjectIterator")
            .setDoc("GAnyObject iterator.")
            .func("hasNext", &GAnyObjectIterator::hasNext)
            .func("next", &GAnyObjectIterator::next)
            .func("remove", &GAnyObjectIterator::remove)
            .func("toFront", &GAnyObjectIterator::toFront);

    GAnyClass::Class < GAnyObjectConstIterator > ()
            ->setName("GAnyObjectConstIterator")
            .setDoc("GAnyObject iterator.")
            .func("hasNext", &GAnyObjectConstIterator::hasNext)
            .func("next", &GAnyObjectConstIterator::next)
            .func("toFront", &GAnyObjectConstIterator::toFront);

    GAnyClass::Class < GAnyFunction > ()
            ->setName("GAnyFunction")
            .setDoc("GAnyFunction")
            .func(MetaFunction::ToString, [](GAnyFunction &self) {
                std::stringstream ss;
                ss << "<" << (self.isMethod() ? "Method" : "Function");
                if (!self.name().empty()) {
                    ss << ": " << self.name();
                }
                ss << " at " << &self << ">";
                return ss.str();
            })
            .func(MetaFunction::EqualTo, [](GAnyFunction &self, GAnyFunction &rh) {
                return &self == &rh;
            })
            .func("name", [](GAnyFunction &self) {
                return self.name();
            })
            .func("isMethod", [](GAnyFunction &self) {
                return self.isMethod();
            })
            .func("compareArgs", &GAnyFunction::compareArgs);

    GAnyClass::Class < GAnyClass > ()
            ->setName("GAnyClass")
            .setDoc("GAnyClass");

    GAnyClass::Class < GAny > ()
            ->setName("GAny")
            .setDoc("GAny");

    GAnyClass::Class < GAnyException > ()
            ->setName("GAnyException")
            .setDoc("GAnyException")
            .func(MetaFunction::ToString, [](GAnyException &self) { return (std::string) self.what(); })
            .func("what", [](GAnyException &self) { return (std::string) self.what(); });

    GAnyClass::Class < GAnyClass::GAnyProperty > ()
            ->setName("GAnyProperty")
            .setDoc("GAnyClass::GAnyProperty")
            .func(MetaFunction::ToObject, &GAnyClass::GAnyProperty::dump);

    GAnyClass::Class < GAnyClass::GAnyEnum > ()
            ->setName("GAnyEnum")
            .setDoc("GAnyClass::GAnyEnum")
            .func(MetaFunction::ToObject, [](GAnyClass::GAnyEnum &self) {
                return self.enumObj;
            })
            .func("forEach", [](GAnyClass::GAnyEnum &self, const GAny &func) {
                // func: function(const std::string &key, const GAny &value)->bool
                const auto &enumObj = self.enumObj.as<GAnyObject>();
                GLockerGuard locker(enumObj.lock);
                for (auto &item: enumObj.var) {
                    auto ret = func(item.first, item.second);
                    if (ret.isBoolean() && !ret.toBool()) {
                        break;
                    }
                }
            })
            .func("iterator", [](GAnyClass::GAnyEnum &self) {
                const auto &enumObj = self.enumObj.as<GAnyObject>();
                return std::make_unique<GAnyObjectConstIterator>(enumObj.var);
            });

    GAnyClass::Class < GAnyCaller > ()
            ->setName("GAnyCaller")
            .setDoc("GAnyCaller");

    GAnyClass::Class < std::pair<const std::string, GAny>>
    ()
            ->setName("GAnyObjectPair")
            .property("first", [](std::pair<const std::string, GAny> &self) -> std::string {
                return self.first;
            })
            .property("second", [](std::pair<const std::string, GAny> &self) -> GAny {
                return self.second;
            })
            .property("key", [](std::pair<const std::string, GAny> &self) -> std::string {
                return self.first;
            })
            .property("value", [](std::pair<const std::string, GAny> &self) -> GAny {
                return self.second;
            })
            .func(MetaFunction::ToObject, [](std::pair<const std::string, GAny> &self) {
                GAny obj = GAny::object();
                obj[self.first] = self.second;
                return obj;
            })
            .func(MetaFunction::ToString, [](std::pair<const std::string, GAny> &self) {
                GAny obj = GAny::object();
                obj[self.first] = self.second;
                return obj.toString();
            });

    Class<GAnyIteratorItem>("", "GAnyIteratorItem", "GAny iterator item.")
            .func(MetaFunction::Init, [](const GAny &k, const GAny &v) {
                return std::make_pair(k, v);
            })
            .property("first", [](GAnyIteratorItem &self) -> GAny {
                return self.first;
            })
            .property("second", [](GAnyIteratorItem &self) -> GAny {
                return self.second;
            })
            .property("key", [](GAnyIteratorItem &self) -> GAny {
                return self.first;
            })
            .property("value", [](GAnyIteratorItem &self) -> GAny {
                return self.second;
            })
            .func(MetaFunction::ToObject, [](GAnyIteratorItem &self) {
                GAny obj = GAny::array();
                obj.pushBack(self.first);
                obj.pushBack(self.second);
                return obj;
            })
            .func(MetaFunction::ToString, [](GAnyIteratorItem &self) {
                GAny obj = GAny::array();
                obj.pushBack(self.first);
                obj.pushBack(self.second);
                return obj.toString();
            });

    GAnyClass::Class < GAnyEnvObject > ()
            ->setName("GAnyEnvObject")
            .func(MetaFunction::ToObject, [](GAnyEnvObject &self) {
                return GAny(self.var);
            })
            .func(MetaFunction::ToString, [](GAnyEnvObject &self) {
                return GAny(self.var).toString();
            })
            .func(MetaFunction::GetItem, &GAnyEnvObject::get)
            .func("contains", &GAnyEnvObject::contains)
            .func("forEach", [](GAnyEnvObject &self, const GAny &func) {
                // func: function(const std::string &key, const GAny &value)->bool
                GLockerGuard locker(self.lock);
                for (auto &item: self.var) {
                    auto ret = func(item.first, item.second);
                    if (ret.isBoolean() && !ret.toBool()) {
                        break;
                    }
                }
            })
            .func("iterator", [](GAnyEnvObject &self) {
                return std::make_unique<GAnyObjectIterator>(self.var);
            });

    REF_ENUM(AnyType, "", "GAny Enum AnyType.");
    REF_ENUM(MetaFunction, "", "GAny Enum MetaFunction.");

    auto AnyClass = GAnyClass::Class("", "AnyClass", "Class Registration Tool.");
    GAnyClass::registerToEnv(AnyClass);
    AnyClass->staticFunc(
                    MetaFunction::Init,
                    [](GAny &self,
                       const std::string &nameSpace,
                       const std::string &name,
                       const std::string &doc) {
                        self["clazz"] = GAnyClass::Class(nameSpace, name, doc);
                    })
            .func("getClass", [](GAny &self) {
                return self["clazz"];
            })
            .func("func", [](GAny &self, const std::string &name, const GAny &function) {
                self["clazz"].as<GAnyClass>().func(name, function);
                return self;
            })
            .func("func", [](GAny &self, const std::string &name, const GAny &function, const std::string &doc) {
                self["clazz"].as<GAnyClass>().func(name, function, doc);
                return self;
            })
            .func("func", [](GAny &self, MetaFunction metaFunc, const GAny &function) {
                self["clazz"].as<GAnyClass>().func(metaFunc, function);
                return self;
            })
            .func("func", [](GAny &self, MetaFunction metaFunc, const GAny &function, const std::string &doc) {
                self["clazz"].as<GAnyClass>().func(metaFunc, function, doc);
                return self;
            })
            .func("staticFunc", [](GAny &self, const std::string &name, const GAny &function) {
                self["clazz"].as<GAnyClass>().staticFunc(name, function);
                return self;
            })
            .func("staticFunc", [](GAny &self, const std::string &name, const GAny &function, const std::string &doc) {
                self["clazz"].as<GAnyClass>().staticFunc(name, function, doc);
                return self;
            })
            .func("staticFunc", [](GAny &self, MetaFunction metaFunc, const GAny &function) {
                self["clazz"].as<GAnyClass>().staticFunc(metaFunc, function);
                return self;
            })
            .func("staticFunc",
                 [](GAny &self, MetaFunction metaFunc, const GAny &function, const std::string &doc) {
                     self["clazz"].as<GAnyClass>().staticFunc(metaFunc, function, doc);
                     return self;
                 })
            .func("property", [](GAny &self, const std::string &name,
                                   const GAny &fGet, const GAny &fSet) {
                self["clazz"].as<GAnyClass>().property(name, fGet, fSet);
                return self;
            })
            .func("property", [](GAny &self, const std::string &name,
                                   const GAny &fGet, const GAny &fSet,
                                   const std::string &doc) {
                self["clazz"].as<GAnyClass>().property(name, fGet, fSet, doc);
                return self;
            })
            .func("defEnum", [](GAny &self, const std::string &name, const GAny &enumObj) {
                self["clazz"].as<GAnyClass>().defEnum(name, enumObj);
                return self;
            })
            .func("defEnum", [](GAny &self, const std::string &name, const GAny &enumObj, const std::string &doc) {
                self["clazz"].as<GAnyClass>().defEnum(name, enumObj, doc);
                return self;
            })
            .func("inherit", [](GAny &self, const GAny &parent) {
                if (parent.is<GAnyClass>()) {
                    self["clazz"].as<GAnyClass>().inherit(parent);
                } else if (parent.is("AnyClass")) {
                    self["clazz"].as<GAnyClass>().inherit(parent["clazz"]);
                }
                return self;
            })
            .staticFunc("registerToEnv", &GAnyClass::registerToEnv);

    refGString();
}

#include "gany_pfn_impl.h"

void initGAnyCore()
{
    using namespace gx;
    RegisterBuiltin(ganyGetEnvImpl, ganyParseJsonImpl, ganyRegisterToEnvImpl, ganyClassInstanceImpl);
    initPluginLoader();
}
