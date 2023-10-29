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

#ifndef GX_GANY_PFN_H
#define GX_GANY_PFN_H

#include "gglobal.h"
#include "gany_module_def.h"


typedef void *(GX_API_PTR *PFN_ganyGetEnv)();
typedef void (GX_API_PTR *PFN_ganyParseJson)(const char *jsonStr, void *ret);
typedef void (GX_API_PTR *PFN_ganyRegisterToEnv)(void *clazz);
typedef void (GX_API_PTR *PFN_ganyClassInstance)(void *typeInfo, void *ret);

extern PFN_ganyGetEnv pfnGanyGetEnv;
extern PFN_ganyParseJson pfnGanyParseJson;
extern PFN_ganyRegisterToEnv pfnGanyRegisterToEnv;
extern PFN_ganyClassInstance pfnGanyClassInstance;

#endif //GX_GANY_PFN_H
