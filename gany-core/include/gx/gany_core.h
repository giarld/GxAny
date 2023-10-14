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

#ifndef GX_GANY_CORE_H
#define GX_GANY_CORE_H

#include "gx/base.h"
#include "gx/gglobal.h"

#include <string>
#include <functional>

#define GANY_IMPORT_MODULE(MODULE_NAME) \
    Register##MODULE_NAME(pfnGanyGetEnv, pfnGanyParseJson, pfnGanyRegisterToEnv, pfnGanyClassInstance)

extern "C" GX_API void GX_API_CALL initGAnyCore();

GX_NS_BEGIN

GX_API void GX_API_CALL setPluginLoaders(const std::string &pluginType,
                             const std::function<bool(const std::string &, const std::string &)> &func);

GX_API void GX_API_CALL setPluginSearchPath(const std::string &path);

GX_API const std::vector<std::string> & GX_API_CALL getPluginSearchPaths();

GX_API bool GX_API_CALL loadPlugin(const std::string &searchPath, const std::string &libName);

GX_API bool GX_API_CALL loadPlugin(const std::string &libName);

GX_NS_END

#endif //GX_GANY_CORE_H
