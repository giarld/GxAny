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

#include "gx/gany_c_api.h"

#include <gx/gany.h>
#include <gx/gmutex.h>

#include <memory.h>


using namespace gx;

static CAnyFunctionProxy sCAnyFunctionProxy = nullptr;
static CAnyFunctionDtorListener sCAnyFunctionDtorListener = nullptr;

static GRWLock sLockFuncProxy;
static GRWLock sLockFDtor;

const static std::string EmptyStr;
static std::vector<std::unique_ptr<std::string>> sStringCache;
static GMutex sStrCacheLock;

static GAny sLogger;

static GAnyPtr ganyCreatePtr(const GAny &v)
{
    return reinterpret_cast<GAnyPtr>(new GAny(v));
}

static GAnyString cacheString(std::string &&str)
{
    GLockerGuard locker(sStrCacheLock);

    sStringCache.push_back(std::make_unique<std::string>(std::move(str)));
    return sStringCache.back()->c_str();
}

static void printLogE(const std::string &log)
{
    if (sLogger.isUndefined()) {
        try {
            sLogger = GEnv.getItem("GLog");
        } catch (GAnyException &) {}
        if (!sLogger.isClass()) {
            sLogger = GAny::null();
        }
    }
    if (sLogger) {
        try {
            sLogger.call("LogE", log);
        } catch (GAnyException &e) {
            fprintf(stderr, "%s", e.what());
        }
    } else {
        fprintf(stderr, "%s", log.c_str());
    }
}


class CAnyFunctionDtorHandler
{
public:
    explicit CAnyFunctionDtorHandler(CAnyFuncPtr funcPtr)
            : mFuncPtr(funcPtr)
    {
    }

    ~CAnyFunctionDtorHandler()
    {
        auto locker = sLockFDtor.readGuard();
        if (sCAnyFunctionDtorListener) {
            sCAnyFunctionDtorListener(mFuncPtr);
        }
    }

private:
    CAnyFuncPtr mFuncPtr;
};

void ganyFreeString(GAnyString str)
{
    if (str == EmptyStr.c_str()) {
        return;
    }
    GLockerGuard locker(sStrCacheLock);
    auto it = std::find_if(sStringCache.begin(), sStringCache.end(), [&](const auto &s) {
        return str == s->c_str();
    });
    if (it != sStringCache.end()) {
        if (sStringCache.size() > 1) {
            std::swap(*it, *(sStringCache.end() - 1));
        }
        sStringCache.pop_back();
    }
}

void ganySetFunctionProxy(CAnyFunctionProxy proxy)
{
    auto locker = sLockFuncProxy.writeGuard();
    sCAnyFunctionProxy = proxy;
}

void ganySetFunctionDtorListener(CAnyFunctionDtorListener listener)
{
    auto locker = sLockFDtor.writeGuard();
    sCAnyFunctionDtorListener = listener;
}


GAnyPtr ganyCreate(GAnyPtr v)
{
    return ganyCreatePtr(*reinterpret_cast<const GAny *>(v));
}

GAnyPtr ganyCreateBool(bool v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateInt32(int32_t v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateInt64(int64_t v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateInt8(int8_t v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateInt16(int16_t v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateFloat(float v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateDouble(double v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateString(const char *v)
{
    return ganyCreatePtr(std::string(v));
}

GAnyPtr ganyCreatePointer(void *v)
{
    return ganyCreatePtr(GAny((GAnyBytePtr) v));
}

GAnyPtr ganyCreateArray()
{
    return ganyCreatePtr(GAny::array());
}

GAnyPtr ganyCreateObject()
{
    return ganyCreatePtr(GAny::object());
}

GAnyPtr ganyCreateFunction(CAnyFuncPtr funcPtr)
{
    auto dtorHandler = std::make_shared<CAnyFunctionDtorHandler>(funcPtr);

    GAnyFunction func = GAnyFunction::createVariadicFunction(
            "CFunction", "C GAny function",
            [funcPtr, dtorHandler](const GAny **args, int32_t argc) {
                try {
                    auto *tArgs = (GAnyPtr *) alloca(sizeof(GAnyPtr) * argc);
                    for (int32_t i = 0; i < argc; i++) {
                        // Stored by the target language and released at the appropriate time.
                        tArgs[i] = ganyCreatePtr(*args[i]);
                    }

                    GAnyPtr ret = 0;
                    {
                        auto locker = sLockFuncProxy.readGuard();
                        if (sCAnyFunctionProxy) {
                            ret = sCAnyFunctionProxy(funcPtr, tArgs, argc);
                        }
                    }
                    if (ret == 0) {
                        return GAny::null();
                    }
                    GAny retCopy = *reinterpret_cast<GAny *>(ret);
                    ganyDestroy(ret);

                    return retCopy;
                } catch (const std::exception &e) {
                    printLogE(e.what());
                    return GAny::undefined();
                }
            });
    return ganyCreatePtr(GAny(func));
}

GAnyPtr ganyCreateUndefined()
{
    return ganyCreatePtr(GAny::undefined());
}

GAnyPtr ganyCreateNull()
{
    return ganyCreatePtr(GAny::null());
}

void ganyDestroy(GAnyPtr any)
{
    if (!any) {
        return;
    }
    auto *anyPtr = reinterpret_cast<GAny *>(any);
    delete anyPtr;
}

GAnyPtr ganyEnvironment()
{
    return ganyCreatePtr(GAny::environment());
}


GAnyPtr ganyClone(GAnyPtr any)
{
    try {
        return ganyCreatePtr(reinterpret_cast<GAny *>(any)->clone());
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreatePtr(GAny::undefined());
    }
}

const char *ganyClassTypeName(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->classTypeName().c_str();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return "";
    }
}

const char *ganyTypeName(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->typeName().c_str();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return "";
    }
}

int32_t ganyLength(GAnyPtr any)
{
    try {
        return (int32_t) reinterpret_cast<GAny *>(any)->length();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return 0;
    }
}

int32_t ganySize(GAnyPtr any)
{
    try {
        return (int32_t) reinterpret_cast<GAny *>(any)->size();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return 0;
    }
}

bool ganyIs(GAnyPtr any, const char *typeStr)
{
    try {
        return reinterpret_cast<GAny *>(any)->is(typeStr);
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsUndefined(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isUndefined();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsNull(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isNull();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsFunction(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isFunction();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsClass(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isClass();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsException(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isException();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsProperty(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isProperty();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsEnum(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isEnum();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsObject(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isObject();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsArray(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isArray();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsInt32(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isInt32();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsInt64(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isInt64();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsInt8(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isInt8();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsInt16(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isInt16();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsFloat(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isFloat();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsDouble(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isDouble();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsNumber(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isNumber();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsString(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isString();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsBoolean(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isBoolean();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsUserObject(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->isUserObject();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyIsPointer(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->is<GAnyBytePtr>();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

int32_t ganyToInt32(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->toInt32();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return 0;
    }
}

int64_t ganyToInt64(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->toInt64();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return 0;
    }
}

int8_t ganyToInt8(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->toInt8();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return 0;
    }
}

int16_t ganyToInt16(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->toInt16();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return 0;
    }
}

float ganyToFloat(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->toFloat();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return 0;
    }
}

double ganyToDouble(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->toDouble();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return 0;
    }
}

bool ganyToBool(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->toBool();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

GAnyString ganyToString(GAnyPtr any)
{
    try {
        std::string str = reinterpret_cast<GAny *>(any)->toString();
        return cacheString(std::move(str));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return EmptyStr.c_str();
    }
}

GAnyString ganyToJsonString(GAnyPtr any, int32_t indent)
{
    try {
        std::string str = reinterpret_cast<GAny *>(any)->toJsonString(indent);
        return cacheString(std::move(str));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return EmptyStr.c_str();
    }
}

GAnyPtr ganyToObject(GAnyPtr any)
{
    try {
        return ganyCreatePtr(reinterpret_cast<GAny *>(any)->toObject());
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreatePtr(GAny::undefined());
    }
}

GAnyPtr ganyParseJson(const char *json)
{
    try {
        return ganyCreatePtr(GAny::parseJson(json));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreatePtr(GAny::object());
    }
}

void *ganyToPointer(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->castAs<GAnyBytePtr>();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return nullptr;
    }
}

GAnyString ganyDump(GAnyPtr any)
{
    try {
        GAny *anyPtr = reinterpret_cast<GAny *>(any);
        std::stringstream ss;
        if (anyPtr->is<GAnyClass>()) {
            ss << anyPtr->as<GAnyClass>();
        } else if (anyPtr->is<GAnyFunction>()) {
            ss << anyPtr->as<GAnyFunction>();
        } else {
            ss << *anyPtr;
        }
        std::string str = ss.str();
        return cacheString(std::move(str));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return EmptyStr.c_str();
    }
}

bool ganyContains(GAnyPtr any, GAnyPtr id)
{
    try {
        GAny idAny = *reinterpret_cast<GAny *>(id);
        return reinterpret_cast<GAny *>(any)->contains(idAny);
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

void ganyErase(GAnyPtr any, GAnyPtr id)
{
    try {
        GAny idAny = *reinterpret_cast<GAny *>(id);
        reinterpret_cast<GAny *>(any)->erase(idAny);
    } catch (const std::exception &e) {
        printLogE(e.what());
    }
}

void ganyPushBack(GAnyPtr any, GAnyPtr rh)
{
    try {
        GAny rhAny = *reinterpret_cast<GAny *>(rh);
        reinterpret_cast<GAny *>(any)->pushBack(rhAny);
    } catch (const std::exception &e) {
        printLogE(e.what());
    }
}

void ganyClear(GAnyPtr any)
{
    try {
        reinterpret_cast<GAny *>(any)->clear();
    } catch (const std::exception &e) {
        printLogE(e.what());
    }
}

GAnyPtr ganyIterator(GAnyPtr any)
{
    try {
        return ganyCreatePtr(reinterpret_cast<GAny *>(any)->iterator());
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreatePtr(GAny::undefined());
    }
}

bool ganyHasNext(GAnyPtr any)
{
    try {
        return reinterpret_cast<GAny *>(any)->hasNext();
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

GAnyPtr ganyNext(GAnyPtr any)
{
    try {
        return ganyCreatePtr(reinterpret_cast<GAny *>(any)->next());
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreatePtr(GAny::undefined());
    }
}


GAnyPtr ganyCallMethod(GAnyPtr any, const char *methodName, GAnyPtr *args, int32_t argc)
{
    try {
        std::vector<GAny> argv;
        for (int32_t i = 0; i < argc; i++) {
            argv.push_back(*reinterpret_cast<GAny *>(args[i]));
        }
        GAny ret = reinterpret_cast<GAny *>(any)->_call(methodName, argv);
        return ganyCreatePtr(ret);
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyCallFunction(GAnyPtr any, GAnyPtr *args, int32_t argc)
{
    try {
        std::vector<GAny> argv;
        for (int32_t i = 0; i < argc; i++) {
            argv.push_back(*reinterpret_cast<GAny *>(args[i]));
        }
        GAny ret = reinterpret_cast<GAny *>(any)->_call(argv);
        return ganyCreatePtr(ret);
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}


GAnyPtr ganyGetItem(GAnyPtr any, GAnyPtr i)
{
    try {
        GAny iAny = *reinterpret_cast<GAny *>(i);
        GAny ret = reinterpret_cast<GAny *>(any)->getItem(iAny);
        return ganyCreatePtr(ret);
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

void ganySetItem(GAnyPtr any, GAnyPtr i, GAnyPtr v)
{
    try {
        GAny iAny = *reinterpret_cast<GAny *>(i);
        GAny vAny = *reinterpret_cast<GAny *>(v);
        reinterpret_cast<GAny *>(any)->setItem(iAny, vAny);
    } catch (const std::exception &e) {
        printLogE(e.what());
    }
}

void ganyDelItem(GAnyPtr any, GAnyPtr i)
{
    try {
        GAny iAny = *reinterpret_cast<GAny *>(i);
        reinterpret_cast<GAny *>(any)->delItem(iAny);
    } catch (const std::exception &e) {
        printLogE(e.what());
    }
}


GAnyPtr ganyOperatorNeg(GAnyPtr any)
{
    try {
        return ganyCreatePtr(-(*reinterpret_cast<GAny *>(any)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyOperatorAdd(GAnyPtr a, GAnyPtr b)
{
    try {
        return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) + (*reinterpret_cast<GAny *>(b)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyOperatorSub(GAnyPtr a, GAnyPtr b)
{
    try {
        return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) - (*reinterpret_cast<GAny *>(b)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyOperatorMul(GAnyPtr a, GAnyPtr b)
{
    try {
        return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) * (*reinterpret_cast<GAny *>(b)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyOperatorDiv(GAnyPtr a, GAnyPtr b)
{
    try {
        return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) / (*reinterpret_cast<GAny *>(b)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyOperatorMod(GAnyPtr a, GAnyPtr b)
{
    try {
        return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) % (*reinterpret_cast<GAny *>(b)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyOperatorBitXor(GAnyPtr a, GAnyPtr b)
{
    try {
        return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) ^ (*reinterpret_cast<GAny *>(b)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyOperatorBitOr(GAnyPtr a, GAnyPtr b)
{
    try {
        return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) | (*reinterpret_cast<GAny *>(b)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyOperatorBitAnd(GAnyPtr a, GAnyPtr b)
{
    try {
        return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) & (*reinterpret_cast<GAny *>(b)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

GAnyPtr ganyOperatorBitNot(GAnyPtr v)
{
    try {
        return ganyCreatePtr(~(*reinterpret_cast<GAny *>(v)));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return ganyCreateUndefined();
    }
}

bool ganyOperatorEqualTo(GAnyPtr a, GAnyPtr b)
{
    try {
        return (*reinterpret_cast<GAny *>(a)) == (*reinterpret_cast<GAny *>(b));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

bool ganyOperatorLessThan(GAnyPtr a, GAnyPtr b)
{
    try {
        return (*reinterpret_cast<GAny *>(a)) < (*reinterpret_cast<GAny *>(b));
    } catch (const std::exception &e) {
        printLogE(e.what());
        return false;
    }
}

REGISTER_GANY_MODULE(GAnyC)
{
    // Nothing, just register pfn
}
