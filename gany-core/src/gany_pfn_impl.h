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

#ifndef GX_GANY_PFN_IMPL_H
#define GX_GANY_PFN_IMPL_H

#include "gx/gany.h"


GX_NS_BEGIN

void *GX_API_CALL ganyGetEnvImpl();

void GX_API_CALL ganyParseJsonImpl(const char *jsonStr, void *ret);

void GX_API_CALL ganyRegisterToEnvImpl(void *clazz);

void GX_API_CALL ganyClassInstanceImpl(void *cppType, void *ret);

GX_NS_END

#endif //GX_GANY_PFN_IMPL_H