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

#include "gany_pfn_impl.h"

#include "gany_env_object.h"
#include "gx/sstring.h"

#include <rapidjson/document.h>


GX_NS_BEGIN

GAny &getEnv()
{
    static GAny _env = std::make_unique<GAnyEnvObject>();
    auto &env = _env.as<GAnyEnvObject>();
    if (!env.contains("_user")) {
        GAny userData = GAny::object();
        env.set("_user", userData);
    }
    return _env;
}

GAnyEnvObject &getEnvObject()
{
    return getEnv().as<GAnyEnvObject>();
}

void *ganyGetEnvImpl()
{
    return &getEnv();
}

static GAny _parseJson(const rapidjson::Value &v)
{
    using namespace rapidjson;

    switch (v.GetType()) {
        case rapidjson::kNullType:
            return GAny::null();
        case rapidjson::kFalseType:
            return false;
        case rapidjson::kTrueType:
            return true;
        case rapidjson::kObjectType: {
            GAny obj = GAny::object();
            for (auto &i: v.GetObject()) {
                obj[std::string(i.name.GetString())] = _parseJson(i.value);
            }
            return obj;
        }
        case rapidjson::kArrayType: {
            GAny array = GAny::array();
            for (const Value &i: v.GetArray()) {
                array.pushBack(_parseJson(i));
            }
            return array;
        }
        case rapidjson::kStringType: {
            auto s = std::string(v.GetString());
            return s;
        }
        case rapidjson::kNumberType: {
            if (v.IsInt()) {
                return v.GetInt();
            }
            if (v.IsUint()) {
                return v.GetUint();
            }
            if (v.IsInt64()) {
                return v.GetInt64();
            }
            if (v.IsUint64()) {
                return v.GetUint64();
            }
            if (v.IsDouble()) {
                return v.GetDouble();
            }
            if (v.IsFloat()) {
                return v.GetFloat();
            }
        }
            break;
    }
    return GAny::object();
}


void ganyParseJsonImpl(const char *jsonStr, void *ret)
{
    using namespace rapidjson;

    Document doc;

    GAny &retObj = *reinterpret_cast<GAny *>(ret);
    if (!doc.Parse(jsonStr).HasParseError()) {
        retObj = _parseJson(doc);
        return;
    }
    retObj = GAny::object();
}

void ganyRegisterToEnvImpl(void *clazz)
{
    std::shared_ptr<GAnyClass> clazzPtr = *reinterpret_cast<std::shared_ptr<GAnyClass> *>(clazz);
    if (clazzPtr->getName().empty()) {
        return;
    }
    auto &env = getEnvObject();
    if (clazzPtr->getNameSpace().empty()) {
        if (!env.contains(clazzPtr->getName())) {
            env.set(clazzPtr->getName(), clazzPtr);
        }
    } else {
        if (!env.contains(clazzPtr->getNameSpace())) {
            env.set(clazzPtr->getNameSpace(), std::make_shared<GAnyEnvObject>());
        }
        auto &ns = env.get(clazzPtr->getNameSpace()).as<GAnyEnvObject>();
        if (!ns.contains(clazzPtr->getName())) {
            ns.set(clazzPtr->getName(), clazzPtr);
        }
    }
}

void ganyClassInstanceImpl(void *cppType, void *ret)
{
    static GSpinLock lock;
    static std::unordered_map<StaticString, std::shared_ptr<GAnyClass>> clsMap;
    static std::array<std::shared_ptr<GAnyClass>, 25> basicTypeArray;

    auto *cppTypePtr = reinterpret_cast<CppType *>(cppType);
    std::shared_ptr<GAnyClass> &cls = *reinterpret_cast<std::shared_ptr<GAnyClass> *>(ret);

    if (cppTypePtr->basicTypeIndex() >= 0) {
        auto &basicTypeRef = basicTypeArray[cppTypePtr->basicTypeIndex()];
        if (!basicTypeRef) {
            basicTypeRef = std::shared_ptr<GAnyClass>(
                    GX_NEW(GAnyClass, "", cppTypePtr->demangleName(), "", *cppTypePtr));
        }
        cls = basicTypeRef;
        return;
    }

    StaticString className = cppTypePtr->name();

    std::lock_guard locker(lock);
    auto it = clsMap.find(className);
    if (it != clsMap.end()) {
        cls = it->second;
        return;
    }
    cls = std::shared_ptr<GAnyClass>(GX_NEW(GAnyClass, "", cppTypePtr->demangleName(), "", *cppTypePtr));
    clsMap.emplace(className, cls);
}

GX_NS_END