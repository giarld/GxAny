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

#ifndef GX_GANY_H
#define GX_GANY_H

#include "base.h"
#include "platform.h"
#include "gany_pfn.h"

#include "enum.h"
#include "common.h"
#include "gstring.h"
#include "gmutex.h"

#include <cstring>
#include <string>
#include <utility>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <memory>
#include <array>
#include <tuple>
#include <typeindex>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <math.h>


#if GX_COMPILER_GCC || (GX_COMPILER_CLANG && !GX_PLATFORM_WINDOWS)

#include <cxxabi.h>

#endif

#if defined(BUILD_SHARED_LIBS) || defined(IS_GANY_CORE)
#define REGISTER_GANY_MODULE(MODULE_NAME) \
    PFN_ganyGetEnv pfnGanyGetEnv = nullptr;  \
    PFN_ganyParseJson pfnGanyParseJson = nullptr;    \
    PFN_ganyRegisterToEnv pfnGanyRegisterToEnv = nullptr;    \
    PFN_ganyClassInstance pfnGanyClassInstance = nullptr;    \
    class GAnyModule##MODULE_NAME         \
    {                                     \
    public:                               \
        GAnyModule##MODULE_NAME();        \
    };                                    \
    int32_t Register##MODULE_NAME(int64_t versionCode, PFN_ganyGetEnv pfnGetEnv, PFN_ganyParseJson pfnParseJson, PFN_ganyRegisterToEnv pfnRegisterToEnv, PFN_ganyClassInstance pfnClassInstance) \
    {                                     \
        if (versionCode != GANY_VERSION_CODE) {  \
            return 1;                     \
        }                                 \
        pfnGanyGetEnv = pfnGetEnv;        \
        pfnGanyParseJson = pfnParseJson;  \
        pfnGanyRegisterToEnv = pfnRegisterToEnv; \
        pfnGanyClassInstance = pfnClassInstance; \
        static GAnyModule##MODULE_NAME gAnyModule##MODULE_NAME; \
        gAnyModule##MODULE_NAME;          \
        return 0;                         \
    }                                     \
    GAnyModule##MODULE_NAME::GAnyModule##MODULE_NAME()
#else
#define REGISTER_GANY_MODULE(MODULE_NAME) \
    class GAnyModule##MODULE_NAME         \
    {                                     \
    public:                               \
        GAnyModule##MODULE_NAME();        \
    };                                    \
    int32_t Register##MODULE_NAME(int64_t versionCode, PFN_ganyGetEnv, PFN_ganyParseJson, PFN_ganyRegisterToEnv, PFN_ganyClassInstance) \
    {                                     \
        if (versionCode != GANY_VERSION_CODE) { \
            return 1;                     \
        }                                 \
        static GAnyModule##MODULE_NAME gAnyModule##MODULE_NAME; \
        gAnyModule##MODULE_NAME;          \
        return 0;                         \
    }                                     \
    GAnyModule##MODULE_NAME::GAnyModule##MODULE_NAME()
#endif

#define GANY_VERSION_MAJOR 1
#define GANY_VERSION_MINOR 0
#define GANY_VERSION_PATCH 3

// (0 | (GANY_VERSION_PATCH << 16) | ((uint64_t) GANY_VERSION_MINOR << 32) | ((uint64_t) GANY_VERSION_MAJOR << 48))
#define GANY_VERSION_CODE 0x1000000030000

#define GEnv gx::GAny::environment()


GX_NS_BEGIN

namespace detail
{
template<bool BL, typename T = void> using enable_if_t = typename std::enable_if<BL, T>::type;

template<bool B, typename T, typename F> using conditional_t = typename std::conditional<B, T, F>::type;
template<typename T> using remove_cv_t = typename std::remove_cv<T>::type;
template<typename T> using remove_reference_t = typename std::remove_reference<T>::type;

template<typename T>
struct remove_class
{
};
template<typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...)>
{
    typedef R type(A...);
};
template<typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...) const>
{
    typedef R type(A...);
};

template<typename F>
struct strip_function_object
{
    using type = typename remove_class<decltype(&F::operator())>::type;
};

template<typename Function, typename F = remove_reference_t<Function>>
using function_signature_t = conditional_t<
        std::is_function<F>::value,
        F,
        typename conditional_t<
                std::is_pointer<F>::value || std::is_member_pointer<F>::value,
                std::remove_pointer<F>,
                strip_function_object<F>
        >::type
>;

template<size_t ...>
struct index_sequence
{
};
template<size_t N, size_t ...S>
struct make_index_sequence_impl : make_index_sequence_impl<N - 1, N - 1, S...>
{
};
template<size_t ...S>
struct make_index_sequence_impl<0, S...>
{
    typedef index_sequence<S...> type;
};
template<size_t N> using make_index_sequence = typename make_index_sequence_impl<N>::type;

template<class T>
struct sfinae_true : public std::true_type
{
    typedef T type;
};

template<class T>
struct sfinae_false : public std::false_type
{
    typedef T type;
};

template<class T>
static auto test_call_op(int) -> sfinae_true<decltype(&T::operator())>;

template<class T>
static auto test_call_op(long) -> sfinae_false<T>;

template<class T, class T2 =decltype(test_call_op<T>(0))>
struct has_call_op_ : public T2
{
};

template<class T>
struct has_call_op : public std::is_member_function_pointer<typename has_call_op_<T>::type>
{
};

template<typename T>
struct is_callable : public has_call_op<T>
{
};
template<typename TClass, typename TRet, typename... TArgs>
struct is_callable<TRet(TClass::*)(TArgs...)>
{
    const static bool value = true;
};

template<typename TClass, typename TRet, typename... TArgs>
struct is_callable<TRet(TClass::*)(TArgs...) const>
{
    const static bool value = true;
};

template<typename TRet, typename... TArgs>
struct is_callable<TRet(*)(TArgs...)>
{
    const static bool value = true;
};

}

class GAny;

class GAnyValue;

class GAnyFunction;

class GAnyClass;

class GAnyObject;

class GAnyArray;

class GAnyException;

class GAnyCaller;

class CppType;

template<typename T>
class GAnyValueP;

using GAnyBytePtr = char *;

using GAnyConstBytePtr = const char *;


DEF_ENUM_19(AnyType, 0,
            undefined_t,    ///< undefined (void)
            null_t,         ///< null (nullptr)
            boolean_t,      ///< boolean (bool)
            int8_t,         ///< int8  (int8_t)
            int16_t,        ///< int16  (int16_t)
            int32_t,        ///< int32  (int32_t)
            int64_t,        ///< int64  (int64_t)
            float_t,        ///< number (float)
            double_t,       ///< number (double)
            string_t,       ///< string
            array_t,        ///< array (ordered collection of values)
            object_t,       ///< object (unordered set of name/value pairs)
            function_t,     ///< function expressions
            class_t,        ///< class expressions
            property_t,     ///< class property
            enum_t,         ///< enum
            exception_t,    ///< exception (GAnyException)
            user_obj_t,     ///< user object
            caller_t        ///< user object method caller
)

inline const std::array<std::string, EnumAnyTypeCount> &anyTypeNames()
{
    static const std::array<std::string, EnumAnyTypeCount> names = {
            "undefined",
            "null",
            "boolean",
            "int8",
            "int16",
            "int32",
            "int64",
            "float",
            "double",
            "string",
            "array",
            "object",
            "function",
            "class",
            "property",
            "enum",
            "exception",
            "user_obj",
            "caller"
    };
    return names;
}


DEF_ENUM_23(MetaFunction, 0,
            Init,
            Negate,
            Addition,
            Subtraction,
            Multiplication,
            Division,
            Modulo,
            BitXor,
            BitOr,
            BitAnd,
            BitNot,
            EqualTo,
            LessThan,
            GetItem,
            SetItem,
            DelItem,
            Length,
            ToString,
            ToInt32,
            ToInt64,
            ToDouble,
            ToBoolean,
            ToObject
)

inline const std::array<std::string, EnumMetaFunctionCount> &metaFunctionNames()
{
    static const std::array<std::string, EnumMetaFunctionCount> names = {
            "__init",
            "__neg",
            "__add",
            "__sub",
            "__mul",
            "__div",
            "__mod",
            "__xor",
            "__or",
            "__and",
            "__not",
            "__eq",
            "__lt",
            "__getitem",
            "__setitem",
            "__delitem",
            "__len",
            "__str",
            "__to_int32",
            "__to_int64",
            "__to_double",
            "__to_bool",
            "__to_object"
    };
    return names;
}


using GAnyIteratorItem = std::pair<GAny, GAny>;

class GAny
{
public:
    GAny()
            : GAny(undefined())
    {}

    GAny(std::shared_ptr<GAnyValue> v)
            : mVal(std::move(v))
    {}

    template<typename T>
    GAny(const T &var);

    template<typename T>
    GAny(detail::enable_if_t<!std::is_same<T, GAny>::value, T> &&var);

    template<typename T>
    GAny(std::unique_ptr<T> &&v);

    GAny(const std::initializer_list<GAny> &init);

    template<typename Return, typename... Args>
    GAny(Return (*f)(Args...), const std::string &doc = "");

    template<typename Return, typename Class, typename... arg>
    GAny(Return (Class::*f)(arg...), const std::string &doc = "");

    template<typename Return, typename Class, typename... arg>
    GAny(Return (Class::*f)(arg...) const, const std::string &doc = "");

public:
    template<class T>
    static GAny create(const T &t);

    template<class T>
    static GAny create(T &&t);

    static GAny object(const std::map<std::string, GAny> &m);

    static GAny object(const std::unordered_map<std::string, GAny> &m = {});

    static GAny array(const std::vector<GAny> &vec = {});

    static GAny array(const std::list<GAny> &lst);

    static GAny undefined();

    static GAny null();

    static const GAny environment();

public:
    const std::shared_ptr<GAnyValue> &value() const;

    GAny clone() const;

    const std::string &classTypeName() const;

    const std::string &typeName() const;

    CppType cppType() const;

    AnyType type() const;

    GAnyClass &classObject() const;

    size_t length() const;

    size_t size() const
    {
        return length();
    }

public:
    template<typename T>
    bool is() const;

    bool is(const CppType &cppType) const;

    bool is(const std::string &typeStr) const;

    bool isUndefined() const;

    bool isNull() const;

    bool isFunction() const;

    bool isClass() const;

    bool isException() const;

    bool isProperty() const;

    bool isEnum() const;

    bool isObject() const;

    bool isArray() const;

    bool isInt8() const;

    bool isInt16() const;

    bool isInt32() const;

    bool isInt64() const;

    bool isFloat() const;

    bool isDouble() const;

    bool isNumber() const;

    bool isString() const;

    bool isBoolean() const;

    bool isUserObject() const;

    bool isCaller() const;

public:
    template<typename T>
    const T &as() const;

    template<typename T>
    T &as();

    template<typename T>
    T &unsafeAs();

    template<typename T>
    const T &unsafeAs() const;

    template<typename T>
    detail::enable_if_t<std::is_pointer<T>::value, T>
    castAs();

    template<typename T>
    detail::enable_if_t<std::is_reference<T>::value, T &>
    castAs();

    template<typename T>
    detail::enable_if_t<!std::is_reference<T>::value && !std::is_pointer<T>::value, T>
    castAs() const;

    template<typename T>
    T get()
    {
        return castAs<T>();
    }

    template<typename T>
    T get(const std::string &name, T def);

    template<typename T>
    void set(const std::string &name, const T &v);

    GAny get(const std::string &name, GAny def = GAny())
    {
        return get<GAny>(name, std::move(def));
    }

    const void *getPointer() const;

    void *getPointer(const std::string &name, void *def = nullptr);

    /**
     * @brief Determine the existence of elements (only applicable to array (check if the corresponding element exists),
     *          object (check if the value for a given key exists),
     *          and environment (check if the value for a given key exists)).
     * @param id    Element IDs, array(GAny), object keys(string)
     * @return
     */
    bool contains(const GAny &id) const;

    /**
     * @brief Delete element (arrays and objects only).
     * @param id    Element ID, array index (int32), object key (string)
     */
    void erase(const GAny &id);

    /**
     * @brief Add an element to the array.
     * @param rh
     */
    void pushBack(const GAny &rh);

    void clear();

    GAny iterator() const;

    bool hasNext() const;

    GAnyIteratorItem next() const;

public:
    GAny *overload(GAny func);

    template<typename... Args>
    GAny call(const std::string &function, Args &&... args) const;

    template<typename... Args>
    GAny call(MetaFunction metaFunc, Args &&... args) const
    {
        return call(metaFunctionNames()[(size_t) metaFunc], std::forward<Args>(args)...);
    }

    GAny _call(const std::string &function, const GAny **args, int32_t argc) const;

    GAny _call(const std::string &function, std::vector<GAny> &args) const;

    GAny _call(MetaFunction metaFunc, const GAny **args, int32_t argc) const;

    GAny _call(MetaFunction metaFunc, std::vector<GAny> &args) const;

    GAny _call(const GAny **args, int32_t argc) const;

    GAny _call(std::vector<GAny> &args) const;

    template<typename... Args>
    GAny operator()(Args... args) const;

public:
    GAny operator-() const;                 // Negate

    GAny operator+(const GAny &rh) const;   // Addition

    GAny operator-(const GAny &rh) const;   // Subtraction

    GAny operator*(const GAny &rh) const;   // Multiplication

    GAny operator/(const GAny &rh) const;   // Division

    GAny operator%(const GAny &rh) const;   // Modulo

    GAny operator^(const GAny &rh) const;   // BitXor

    GAny operator|(const GAny &rh) const;   // BitOr

    GAny operator&(const GAny &rh) const;   // BitAnd

    GAny operator~() const;                 // BitNot

    bool operator==(const GAny &rh) const;  // EqualTo

    bool operator<(const GAny &rh) const;   // LessThan

    bool operator!=(const GAny &rh) const
    {
        return !((*this) == rh);
    }

    bool operator>(const GAny &rh) const
    {
        return !((*this) == rh || (*this) < rh);
    }

    bool operator<=(const GAny &rh) const
    {
        return ((*this) < rh || (*this) == rh);
    }

    bool operator>=(const GAny &rh) const
    {
        return !((*this) < rh);
    }

    bool operator==(std::nullptr_t) const;

    bool operator!=(std::nullptr_t) const;

    explicit operator bool() const noexcept
    {
        try {
            return toBool();
        } catch (std::exception &) {
        }
        return true;
    }

    GAny &operator+=(const GAny &rh)
    {
        *this = *this + rh;
        return *this;
    }

    GAny &operator-=(const GAny &rh)
    {
        *this = *this - rh;
        return *this;
    }

    GAny &operator*=(const GAny &rh)
    {
        *this = *this * rh;
        return *this;
    }

    GAny &operator/=(const GAny &rh)
    {
        *this = *this / rh;
        return *this;
    }

    GAny &operator%=(const GAny &rh)
    {
        *this = *this % rh;
        return *this;
    }

    GAny &operator^=(const GAny &rh)
    {
        *this = *this ^ rh;
        return *this;
    }

    GAny &operator|=(const GAny &rh)
    {
        *this = *this | rh;
        return *this;
    }

    GAny &operator&=(const GAny &rh)
    {
        *this = *this & rh;
        return *this;
    }


    GAny getItem(const GAny &i) const;          // GetItem

    void setItem(const GAny &i, const GAny &v); // SetItem

    void delItem(const GAny &i);                // DelItem

    const GAny operator[](const GAny &key) const;

    GAny &operator[](const GAny &key);

    template<typename T>
    detail::enable_if_t<std::is_copy_assignable<T>::value, GAny &> operator=(const T &v)
    {
        if (is<T>()) {
            if (isUserObject()) {
                as<T>() = v;
            } else {
                (*this) = GAny(v);
            }
        } else {
            (*this) = GAny(v);
        }
        return *this;
    }

    template<typename T>
    detail::enable_if_t<!std::is_copy_assignable<T>::value, GAny &> operator=(const T &v)
    {
        (*this) = GAny(v);
        return *this;
    }

    std::string toString() const;

    int8_t toInt8() const;

    int16_t toInt16() const;

    int32_t toInt32() const;

    int64_t toInt64() const;

    float toFloat() const;

    double toDouble() const;

    bool toBool() const;

    GAny toObject() const;


    friend std::ostream &operator<<(std::ostream &ost, const GAny &self)
    {
        return self.dumpJson(ost, 2);
    }

    std::string toJsonString(int indent = -1) const;

    static GAny parseJson(const std::string &json);

private:
    std::ostream &dumpJson(std::ostream &o, int indent = -1, int current_indent = 0) const;

private:
    std::shared_ptr<GAnyValue> mVal;
};


class GAnyException : public std::exception
{
public:
    GAnyException(std::string wt)
            : mWhat(std::move(wt))
    {}

    const char *what() const noexcept override
    {
        return mWhat.c_str();
    }

private:
    std::string mWhat;
};


class GAnyFunction
{
public:
    GAnyFunction()
    {}

    template<typename Return, typename... Args>
    GAnyFunction(Return (*f)(Args...), const std::string &doc = "")
    {
        initialize(f, f, doc);
    }

    template<typename Func>
    GAnyFunction(Func &&f, const std::string &doc = "")
    {
        initialize(
                std::forward<Func>(f),
                (detail::function_signature_t<Func> *) nullptr, doc);
    }

    template<typename Return, typename Class, typename... Arg>
    GAnyFunction(Return (Class::*f)(Arg...), const std::string &doc = "")
    {
        initialize(
                [f](Class *c, Arg... args) -> Return {
                    return (c->*f)(args...);
                },
                (Return (*)(Class *, Arg...)) nullptr, doc);
    }

    template<typename Return, typename Class, typename... Arg>
    GAnyFunction(Return (Class::*f)(Arg...) const, const std::string &doc = "")
    {
        initialize(
                [f](const Class *c, Arg... args) -> Return {
                    return (c->*f)(args...);
                },
                (Return (*)(const Class *, Arg ...)) nullptr, doc);
    }

    static GAnyFunction createVariadicFunction(const std::string &name, const std::string &doc,
                                               std::function<GAny(const GAny **args, int32_t argc)> func);

    GAny _call(const GAny **args, int32_t argc) const;

    template<typename... Args>
    GAny call(Args... args) const
    {
        std::initializer_list<GAny> argv = {
                (GAny(std::move(args)))...
        };

        auto tArgc = (int32_t) argv.size();
        const GAny **tArgs = (const GAny **) alloca(sizeof(GAny *) * tArgc);
        for (int32_t i = 0; i < tArgc; i++) {
            tArgs[i] = &argv.begin()[i];
        }

        return _call(tArgs, tArgc);
    }

    template<typename Func, typename Return, typename... Args>
    void initialize(Func &&f, Return (*)(Args...), const std::string &doc = "");

    template<typename Func, typename Return, typename... Args, size_t... Is>
    detail::enable_if_t<std::is_void<Return>::value, GAny>
    call_impl(Func &&f, Return (*)(Args...), const GAny **args, detail::index_sequence<Is...>)
    {
        f(const_cast<GAny *>(args[Is])->castAs<Args>()...);
        return GAny::undefined();
    }

    template<typename Func, typename Return, typename... Args, size_t... Is>
    detail::enable_if_t<!std::is_void<Return>::value, GAny>
    call_impl(Func &&f, Return (*)(Args...), const GAny **args, detail::index_sequence<Is...>)
    {
        return GAny(f(const_cast<GAny *>(args[Is])->castAs<Args>()...));
    }

    std::string signature() const;

    bool compareArgs(const GAnyFunction &rFunc) const;

    bool matchingArgv(const GAny **args, int32_t argc) const;

    void setName(const std::string &name)
    {
        mName = name;
    }

    const std::string &name() const
    {
        return mName;
    }

    bool isMethod() const
    {
        return mIsMethod;
    }

    friend std::ostream &operator<<(std::ostream &sst, const GAnyFunction &self)
    {
        sst << (self.mName.empty() ? "function" : self.mName) << "(...)";
        const GAnyFunction *overload = &self;
        while (overload) {
            sst << "\n";
            if (!overload->mDoc.empty()) {
                sst << "  // " << overload->mDoc << "\n";
            }
            sst << "  ";
            sst << overload->signature();
            if (!overload->mNext.isFunction()) {
                break;
            }
            overload = &overload->mNext.as<GAnyFunction>();
        }
        return sst;
    }

    GAny dump() const;

    void setBoundData(const GAny &data)
    {
        mBoundData = data;
    }

    GAny getBoundData() const
    {
        return mBoundData;
    }

private:
    friend class GAny;

    friend class GAnyClass;

    template<typename>
    friend
    class Class;

    std::string mName;
    std::string mDoc;
    GAny mNext;
    std::vector<GAny> mArgTypes;

    std::function<GAny(const GAny **args, int32_t argc)> mFunc;
    bool mIsMethod = false;
    bool mDoCheckArgs = true;

    GAny mBoundData;
};


class GAnyCaller
{
public:
    GAnyCaller(GAny obj, std::string methodName)
            : mObj(std::move(obj)), mMethodName(std::move(methodName))
    {}

public:
    GAny call(const GAny **args, int32_t argc) const;

private:
    GAny mObj;
    std::string mMethodName;
};


class CppType
{
public:
    explicit CppType(std::type_index typeIndex);

public:
    std::type_index typeIndex() const
    {
        return mType;
    }

    AnyType anyType() const
    {
        return mAnyType;
    }

    int32_t basicTypeIndex() const
    {
        return mBasicTypeIndex;
    }

    const char *name() const
    {
        return mType.name();
    }

    std::string demangleName() const
    {
        const char *typeName = name();

#if GX_COMPILER_GCC || (GX_COMPILER_CLANG && !GX_PLATFORM_WINDOWS)
        int status;
        char *realname = abi::__cxa_demangle(typeName, 0, 0, &status);
        std::string result(realname);
        free(realname);
#else
        std::string result(typeName);
#endif
        return result;
    }

    bool isClassGAny() const
    {
        return mBasicTypeIndex == 24;
    }

    bool operator==(const CppType &rhs) const
    {
        return EqualType(this->typeIndex(), rhs.typeIndex());
    }

    static bool EqualType(const std::type_index &ta, const std::type_index &tb)
    {
#if GX_PLATFORM_OSX
        return strcmp(ta.name(), tb.name()) == 0;
#else
        return ta == tb;
#endif
    }

protected:
    std::type_index mType;
    AnyType mAnyType = AnyType::user_obj_t;
    int32_t mBasicTypeIndex = -1;
};

template<typename T>
class CppTypeP : public CppType
{
public:
    explicit CppTypeP()
            : CppType(typeid(T))
    {}
};

template<typename T>
struct CppTypeP<std::shared_ptr<T>> : public CppType
{
public:
    explicit CppTypeP()
            : CppType(typeid(T))
    {}
};

template<typename T>
struct CppTypeP<std::unique_ptr<T>> : public CppType
{
public:
    explicit CppTypeP()
            : CppType(typeid(T))
    {}
};


class GAnyClass
{
public:
    class GAnyProperty
    {
    public:
        explicit GAnyProperty(std::string name, std::string doc, GAny fGet, GAny fSet)
                : name(std::move(name)), doc(std::move(doc)), fGet(std::move(fGet)), fSet(std::move(fSet))
        {}

        GAny dump() const
        {
            GAny dumpObj = GAny::object();
            dumpObj["name"] = name;
            dumpObj["doc"] = doc;

            bool acqType = false;
            if (fGet.isFunction()) {
                auto func = fGet.as<GAnyFunction>();
                if (func.mDoCheckArgs && !func.mArgTypes.empty()) {
                    acqType = true;
                    dumpObj["type"] = func.mArgTypes[0].as<GAnyClass>().getName();
                }
            }
            if (!acqType && fSet.isFunction()) {
                auto func = fSet.as<GAnyFunction>();
                if (func.mDoCheckArgs && func.mArgTypes.size() >= 3) {
                    dumpObj["type"] = func.mArgTypes[2].as<GAnyClass>().getName();
                }
            }

            if (fGet.isFunction()) {
                dumpObj["getter"] = true;
            } else {
                dumpObj["getter"] = false;
            }
            if (fSet.isFunction()) {
                dumpObj["setter"] = true;
            } else {
                dumpObj["setter"] = false;
            }

            return dumpObj;
        }

    public:
        std::string name;
        std::string doc;
        GAny fGet;
        GAny fSet;
    };

    class GAnyEnum
    {
    public:
        explicit GAnyEnum(std::string name, std::string doc, GAny enumObj)
                : name(std::move(name)), doc(std::move(doc)), enumObj(std::move(enumObj))
        {}

        GAny dump() const
        {
            GAny dumpObj = GAny::object();
            dumpObj["name"] = name;
            dumpObj["doc"] = doc;

            std::vector<std::pair<std::string, int32_t>> enums;
            enums.reserve(enumObj.size());
            auto enumMap = enumObj.castAs<std::unordered_map<std::string, GAny>>();
            for (const auto &item: enumMap) {
                int32_t v = INT32_MAX;
                try {
                    v = item.second.castAs<int32_t>();
                } catch (GAnyException &) {
                }
                enums.emplace_back(item.first, v);
            }

            std::sort(enums.begin(), enums.end(), [](const auto &lhs, const auto &rhs) {
                return lhs.second < rhs.second;
            });

            GAny itemArray = GAny::array();
            dumpObj["enum"] = itemArray;
            for (const auto &item: enums) {
                GAny itemObj = GAny::object();
                itemArray.pushBack(itemObj);
                itemObj["key"] = item.first;
                if (item.second != INT32_MAX) {
                    itemObj["value"] = item.second;
                }
            }

            return dumpObj;
        }

    public:
        std::string name;
        std::string doc;
        GAny enumObj;   // a GAnyObject
    };

public:
    struct DynamicClassObject
    {
    };

    GAnyClass(std::string nameSpace, std::string name, std::string doc,
              const CppType &cppType = CppTypeP<DynamicClassObject>());

public:
    GAnyClass &setNameSpace(const std::string &ns);

    GAnyClass &setName(const std::string &name);

    GAnyClass &setDoc(const std::string &doc);

    const std::string &getNameSpace() const;

    const std::string &getName() const;

    const std::string &getDoc() const;

    GAnyClass &func(const std::string &name, const GAny &function,
                    const std::string &doc = "", bool isMethod = true);

    GAnyClass &func(MetaFunction metaFunc, const GAny &function,
                    const std::string &doc = "", bool isMethod = true);

    GAnyClass &staticFunc(const std::string &name, const GAny &function, const std::string &doc = "");

    GAnyClass &staticFunc(MetaFunction metaFunc, const GAny &function, const std::string &doc = "");

    template<typename Func>
    GAnyClass &func(const std::string &name, Func &&f, const std::string &doc = "")
    {
        return func(name, GAnyFunction(f), doc, true);
    }

    template<typename Func>
    GAnyClass &func(MetaFunction metaFunc, Func &&f, const std::string &doc = "")
    {
        return func(metaFunctionNames()[(size_t) metaFunc], GAnyFunction(f), doc, true);
    }

    template<typename Func>
    GAnyClass &staticFunc(const std::string &name, Func &&f, const std::string &doc = "")
    {
        return func(name, GAnyFunction(f), doc, false);
    }

    template<typename Func>
    GAnyClass &staticFunc(MetaFunction metaFunc, Func &&f, const std::string &doc = "")
    {
        return func(metaFunctionNames()[(size_t) metaFunc], GAnyFunction(f), doc, false);
    }

    GAnyClass &property(const std::string &name,
                        const GAny &fGet, const GAny &fSet = GAny(),
                        const std::string &doc = "");

    template<typename C, typename D>
    GAnyClass &readWrite(const std::string &name, D C::*pm, const std::string &doc = "")
    {
        GAny fGet = GAnyFunction([pm](const C &c) -> const D & {
            return c.*pm;
        });
        GAny fSet = GAnyFunction([pm](C &c, const D &value) {
            c.*pm = value;
        });
        return property(name, fGet, fSet, doc);
    }

    template<typename C, typename D>
    GAnyClass &readOnly(const std::string &name, D C::*pm, const std::string &doc = "")
    {
        GAny fGet = GAnyFunction([pm](const C &c) -> const D & {
            return c.*pm;
        });
        return property(name, fGet, GAny(), doc);
    }

    GAnyClass &defEnum(const std::string &name, const GAny &enumObj, const std::string &doc = "");

    template<typename Base, typename ChildType>
    GAnyClass &inherit()
    {
        GAny base = {
                GAnyClass::instance<Base>(),
                GAnyFunction(
                        [](ChildType *v) {
                            return dynamic_cast<Base *>(v);
                        })};
        mParents.push_back(base);
        return *this;
    }

    GAnyClass &inherit(const GAny &parent);

    static void registerToEnv(std::shared_ptr<GAnyClass> clazz);

public:
    template<typename... Args>
    GAny call(const GAny &inst, const std::string &function, Args... args) const
    {
        std::initializer_list<GAny> argv = {
                (GAny(std::move(args)))...
        };

        auto tArgc = (int32_t) argv.size();
        const GAny **tArgs = (const GAny **) alloca(sizeof(GAny *) * tArgc);
        for (int32_t i = 0; i < tArgc; i++) {
            tArgs[i] = &argv.begin()[i];
        }

        return _call(inst, function, tArgs, tArgc);
    }

    GAny _call(const GAny &inst, const std::string &function, const GAny **args, int32_t argc) const;

    /**
     * @brief Attempt to convert to the base class type.
     * @param targetClass
     * @param inst          Current instance
     * @return
     */
    GAny castToBase(const GAnyClass &targetClass, const GAny &inst);

public:
    template<typename T>
    static std::shared_ptr<GAnyClass> instance()
    {
        return _instance(CppTypeP<T>());
    }

    template<typename T>
    static std::shared_ptr<GAnyClass> Class()
    {
        return instance<T>();
    }

    static std::shared_ptr<GAnyClass> Class(std::string nameSpace, std::string name, std::string doc)
    {
        return std::shared_ptr<GAnyClass>(GX_NEW(GAnyClass, std::move(nameSpace), std::move(name), std::move(doc)));
    }

    GAny dump() const;

    friend std::ostream &operator<<(std::ostream &ost, const GAnyClass &rh)
    {
        ost << rh.dump();
        return ost;
    }

    friend bool operator==(const GAnyClass &lh, const GAnyClass &rh)
    {
        if (lh.mHash != rh.mHash) {
            return false;
        }
        return lh.mNameSpace == rh.mNameSpace && lh.mName == rh.mName;
    }

    friend bool operator!=(const GAnyClass &lh, const GAnyClass &rh)
    {
        return !(lh == rh);
    }

    bool containsMember(const std::string &name) const;

    bool containsMember(MetaFunction func) const
    {
        return containsMember(metaFunctionNames()[(size_t) func]);
    }

    GAny findMember(const std::string &name) const;

    GAny findMember(MetaFunction func) const
    {
        return findMember(metaFunctionNames()[(size_t) func]);
    }

    const std::vector<GAny> &getParents() const;

    const std::unordered_map<std::string, GAny> &getAttributes() const;

private:
    void makeConstructor(GAny func);

    GAny getAttr(const std::string &name) const;

    GAny getItem(const GAny &inst, const GAny &i) const;

    bool setItem(const GAny &inst, const GAny &i, const GAny &v);

    void updateHash();

private:
    static std::shared_ptr<GAnyClass> _instance(CppType cppType);

private:
    std::string mNameSpace;
    std::string mName;
    std::string mDoc;
    CppType mCppType;
    AnyType mType;
    size_t mHash;
    std::unordered_map<std::string, GAny> mAttr; // Read only when in use
    GAny fInit;
    GAny fGetItem;
    GAny fSetItem;
    std::vector<GAny> mParents;

    friend class GAny;
    friend class GAnyFunction;

    template<typename>
    friend
    class Class;

    template<typename>
    friend
    class caster;
};


template<typename C>
class Class
{
public:
    Class(const std::string &nameSpace, const std::string &name, const std::string &doc)
            : mClazz(GAnyClass::Class < C > ())
    {
        mClazz->setNameSpace(nameSpace);
        mClazz->setName(name);
        mClazz->setDoc(doc);
        GAnyClass::registerToEnv(mClazz);
    }

public:
    Class &func(const std::string &name, const GAny &function, const std::string &doc = "")
    {
        mClazz->func(name, function, doc, true);
        return *this;
    }

    Class &func(MetaFunction metaFunc, const GAny &function, const std::string &doc = "")
    {
        mClazz->func(metaFunctionNames()[(size_t) metaFunc], function, doc, true);
        return *this;
    }

    Class &staticFunc(const std::string &name, const GAny &function, const std::string &doc = "")
    {
        mClazz->func(name, function, doc, false);
        return *this;
    }

    Class &staticFunc(MetaFunction metaFunc, const GAny &function, const std::string &doc = "")
    {
        mClazz->func(metaFunctionNames()[(size_t) metaFunc], function, doc, false);
        return *this;
    }

    template<typename Func>
    Class &func(const std::string &name, Func &&f, const std::string &doc = "")
    {
        mClazz->func(name, GAnyFunction(f), doc, true);
        return *this;
    }

    template<typename Func>
    Class &func(MetaFunction metaFunc, Func &&f, const std::string &doc = "")
    {
        mClazz->func(metaFunctionNames()[(size_t) metaFunc], GAnyFunction(f), doc, true);
        return *this;
    }

    template<typename Func>
    Class &staticFunc(const std::string &name, Func &&f, const std::string &doc = "")
    {
        mClazz->func(name, GAnyFunction(f), doc, false);
        return *this;
    }

    template<typename Func>
    Class &staticFunc(MetaFunction metaFunc, Func &&f, const std::string &doc = "")
    {
        mClazz->func(metaFunctionNames()[(size_t) metaFunc], GAnyFunction(f), doc, false);
        return *this;
    }

    template<typename BaseType>
    Class &inherit()
    {
        mClazz->inherit<BaseType, C>();
        return *this;
    }

    template<typename... Args>
    Class &construct(const std::string &doc = "")
    {
        return staticFunc(MetaFunction::Init, [](Args... args) {
            return std::make_unique<C>(args...);
        }, doc);
    }

    Class &property(const std::string &name,
                    const GAny &fGet, const GAny &fSet = GAny(),
                    const std::string &doc = "")
    {
        mClazz->property(name, fGet, fSet, doc);
        return *this;
    }

    template<typename E, typename D>
    Class &readWrite(const std::string &name, D E::*pm, const std::string &doc = "")
    {
        static_assert(std::is_base_of<E, C>::value, "readWrite requires a class member (or base class member)");

        GAny fGet = GAnyFunction([pm](const C &c) -> const D & {
            return c.*pm;
        });
        GAny fSet = GAnyFunction([pm](C &c, const D &value) {
            c.*pm = value;
        });
        return property(name, fGet, fSet, doc);
    }

    template<typename E, typename D>
    Class &readOnly(const std::string &name, D E::*pm, const std::string &doc = "")
    {
        static_assert(std::is_base_of<E, C>::value, "readOnly requires a class member (or base class member)");

        GAny fGet = GAnyFunction([pm](const C &c) -> const D & {
            return c.*pm;
        });
        return property(name, fGet, GAny(), doc);
    }

    Class &defEnum(const std::string &name, const GAny &enumObj, const std::string &doc = "")
    {
        mClazz->defEnum(name, enumObj, doc);
        return *this;
    }

private:
    std::shared_ptr<GAnyClass> mClazz;
};


class GAnyValue
{
public:
    GAnyValue() = default;

    virtual ~GAnyValue() = default;

public:
    typedef std::type_index TypeID;

    virtual const void *as(const TypeID &tp) const
    {
        if (CppType::EqualType(tp, typeid(void))) {
            return this;
        }
        return nullptr;
    }

    virtual const void *ptr() const
    {
        return this;
    }

    virtual GAnyClass *classObject() const
    {
        if (clazz) {
            return clazz;
        }
        return clazz = GAnyClass::instance<void>().get();
    }

    virtual size_t length() const
    {
        return 0;
    }

    virtual GAny clone() const
    {
        return GAny();
    }

public:
    mutable GAnyClass *clazz = nullptr;
};


template<typename T>
class GAnyValueP : public GAnyValue
{
public:
    explicit GAnyValueP(const T &v)
            : var(v)
    {}

    explicit GAnyValueP(T &&v)
            : var(std::move(v))
    {}

    const void *as(const TypeID &tp) const override
    {
        if (CppType::EqualType(tp, typeid(T))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return &var;
    }

    GAnyClass *classObject() const override
    {
        if (clazz) {
            return clazz;
        }
        return clazz = GAnyClass::instance<T>().get();
    }

    GAny clone() const override
    {
        return var;
    }

public:
    T var;
};


template<>
class GAnyValueP<GAny> : public GAnyValue
{
private:
    GAnyValueP(const GAny &v)
    {}
};


template<typename T>
class GAnyValueP<std::shared_ptr<T>> : public GAnyValue
{
public:
    explicit GAnyValueP(const std::shared_ptr<T> &v)
            : var(v)
    {}

    explicit GAnyValueP(std::shared_ptr<T> &&v)
            : var(std::move(v))
    {}

    GAnyClass *classObject() const override
    {
        if (clazz) {
            return clazz;
        }
        return clazz = GAnyClass::instance<T>().get();
    }

    GAny clone() const override
    {
        return var;
    }

    const void *as(const TypeID &tp) const override
    {
        if (CppType::EqualType(tp, typeid(T))) {
            return var.get();
        } else if (CppType::EqualType(tp, typeid(std::shared_ptr<T>))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return var.get();
    }

protected:
    std::shared_ptr<T> var;
};


template<typename T>
class GAnyValueP<std::unique_ptr<T>> : public GAnyValue
{
public:
    explicit GAnyValueP(std::unique_ptr<T> &&v)
            : var(std::move(v))
    {}

    GAnyClass *classObject() const override
    {
        if (clazz) {
            return clazz;
        }
        return clazz = GAnyClass::instance<T>().get();
    }

    const void *as(const TypeID &tp) const override
    {
        if (CppType::EqualType(tp, typeid(T))) {
            return var.get();
        } else if (CppType::EqualType(tp, typeid(std::unique_ptr<T>))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return var.get();
    }

protected:
    std::unique_ptr<T> var;
};


template<typename T>
class GAnyValueP<T *> : public GAnyValue
{
public:
    explicit GAnyValueP(T *v)
            : var(v)
    {}

    GAnyClass *classObject() const override
    {
        if (clazz) {
            return clazz;
        }
        return clazz = GAnyClass::instance<T>().get();
    }

    const void *as(const TypeID &tp) const override
    {
        if (CppType::EqualType(tp, typeid(T))) {
            return var;
        } else if (CppType::EqualType(tp, typeid(T *))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return var;
    }

protected:
    T *var;
};


class GAnyObject : public GAnyValueP<std::unordered_map<std::string, GAny> >
{
public:
    GAnyObject(const std::map<std::string, GAny> &o)
            : GAnyValueP<std::unordered_map<std::string, GAny>>({})
    {
        var.insert(o.begin(), o.end());
    }

    GAnyObject(const std::unordered_map<std::string, GAny> &o)
            : GAnyValueP<std::unordered_map<std::string, GAny>>(o)
    {
    }

    GAnyObject(std::unordered_map<std::string, GAny> &&o)
            : GAnyValueP<std::unordered_map<std::string, GAny>>(o)
    {
    }

public:
    const void *as(const TypeID &tp) const override
    {
        if (CppType::EqualType(tp, typeid(GAnyObject))) {
            return this;
        }
        if (CppType::EqualType(tp, typeid(std::unordered_map<std::string, GAny>))) {
            return &var;
        }
        return nullptr;
    }

    size_t length() const override
    {
        return var.size();
    }

    GAnyClass *classObject() const override
    {
        if (clazz) {
            return clazz;
        }
        return clazz = GAnyClass::instance<GAnyObject>().get();
    }

    GAny operator[](const std::string &key) const
    {
        std::lock_guard locker(lock);
        auto it = var.find(key);
        if (it == var.end()) {
            return GAny::undefined();
        }
        return it->second;
    }

    void set(const std::string &key, const GAny &value)
    {
        std::lock_guard locker(lock);
        auto it = var.find(key);
        if (it == var.end()) {
            var.insert(std::make_pair(key, value));
        } else {
            it->second = value;
        }
    }

    void erase(const std::string &key)
    {
        std::lock_guard locker(lock);
        auto it = var.find(key);
        if (it != var.end()) {
            var.erase(it);
        }
    }

    bool contains(const std::string &key) const
    {
        std::lock_guard locker(lock);
        return var.find(key) != var.end();
    }

    void clear()
    {
        std::lock_guard locker(lock);
        var.clear();
    }

    GAny clone() const override
    {
        GAny copy = GAny::object();

        for (const auto &i: var) {
            copy.setItem(i.first, i.second.clone());
        }

        return copy;
    }

public:
    mutable GSpinLock lock;
};


class GAnyArray : public GAnyValueP<std::vector<GAny> >
{
public:
    GAnyArray(const std::vector<GAny> &v)
            : GAnyValueP<std::vector<GAny>>(v)
    {}

    GAnyArray(std::vector<GAny> &&v)
            : GAnyValueP<std::vector<GAny>>(std::move(v))
    {}

public:
    GAnyClass *classObject() const override
    {
        if (clazz) {
            return clazz;
        }
        return clazz = GAnyClass::instance<GAnyArray>().get();
    }

    size_t length() const override
    {
        return var.size();
    }

    const void *as(const TypeID &tp) const override
    {
        if (CppType::EqualType(tp, typeid(GAnyArray))) {
            return this;
        }
        if (CppType::EqualType(tp, typeid(std::vector<GAny>))) {
            return &var;
        }
        return nullptr;
    }

    GAny operator[](int32_t i) const
    {
        std::lock_guard locker(lock);
        if (i < var.size()) {
            return var[i];
        }
        return GAny::undefined();
    }

    void push_back(const GAny &v)
    {
        std::lock_guard locker(lock);
        var.push_back(v);
    }

    void set(int32_t i, const GAny &v)
    {
        std::lock_guard locker(lock);
        if (i < var.size()) {
            var[i] = v;
        }
    }

    void erase(int32_t i)
    {
        std::lock_guard locker(lock);
        if (i >= 0 && i < var.size()) {
            var.erase(var.begin() + i);
        }
    }

    bool contains(const GAny &v) const
    {
        std::lock_guard locker(lock);
        try {
            for (const auto &i: var) {
                if (i == v) {
                    return true;
                }
            }
        } catch (GAnyException &) {
        }
        return false;
    }

    void clear()
    {
        std::lock_guard locker(lock);
        var.clear();
    }

    GAny clone() const override
    {
        GAny copy = GAny::array();

        for (const auto &i: var) {
            copy.pushBack(i.clone());
        }

        return copy;
    }

public:
    mutable GSpinLock lock;
};

/// ================================================================================================

template<typename T>
class caster
{
public:
    static GAny from(const GAny &var)
    {
        if (var.is<T>()) {
            return var;
        }

        GAnyClass &destClass = *GAnyClass::Class < T > ();
        if (destClass.fInit.isFunction()) {
            GAny ret = destClass.fInit(var);
            if (ret.is<T>()) {
                return ret;
            }
        }

        return GAny::undefined();
    }

    template<typename T1>
    static detail::enable_if_t<detail::is_callable<T1>::value, GAny> to(const T1 &func)
    {
        return GAnyFunction(func);
    }

    template<typename T1>
    static detail::enable_if_t<!detail::is_callable<T1>::value, GAny> to(const T1 &var)
    {
        return GAny::create(var);
    }

    template<typename T1>
    static detail::enable_if_t<!detail::is_callable<T1>::value, GAny> to(T1 &&var)
    {
        return GAny::create(std::forward<T1>(var));
    }
};


template<typename T>
class caster<std::vector<T> >
{
public:
    static GAny from(const GAny &var)
    {
        if (var.is<std::vector<T>>()) {
            return var;
        }

        if (!var.isArray()) {
            return GAny::undefined();
        }

        std::vector<T> ret;
        ret.reserve(var.length());

        const auto &arrayVar = var.as<GAnyArray>();
        {
            std::lock_guard locker(arrayVar.lock);
            for (const GAny &v: arrayVar.var) {
                ret.push_back(v.castAs<T>());
            }
        }

        return GAny::create(ret);
    }

    static GAny to(const std::vector<T> &var)
    {
        return (std::shared_ptr<GAnyValue>) std::make_shared<GAnyArray>(
                std::vector<GAny>(var.begin(), var.end()));
    }
};

template<>
class caster<std::vector<GAny> >
{
public:
    static GAny from(const GAny &var)
    {
        if (var.is<std::vector<GAny>>()) {
            return var;
        }
        return GAny();
    }

    static GAny to(const std::vector<GAny> &var)
    {
        return (std::shared_ptr<GAnyValue>) std::make_shared<GAnyArray>(var);
    }

    static GAny to(std::vector<GAny> &&var)
    {
        return (std::shared_ptr<GAnyValue>) std::make_shared<GAnyArray>(std::move(var));
    }
};

template<typename T>
class caster<std::list<T> >
{
public:
    static GAny from(const GAny &var)
    {
        if (var.is<std::list<T>>()) {
            return var;
        }

        if (!var.isArray()) {
            return GAny::undefined();
        }

        std::list<T> ret;

        const auto &arrayVar = var.as<GAnyArray>();
        {
            std::lock_guard locker(arrayVar.lock);
            for (const GAny &v: arrayVar.var) {
                ret.push_back(v.castAs<T>());
            }
        }

        return GAny::create(ret);
    }

    static GAny to(const std::list<T> &var)
    {
        return (std::shared_ptr<GAnyValue>) std::make_shared<GAnyArray>(
                std::vector<GAny>(var.begin(), var.end()));
    }
};

template<typename T>
class caster<std::map<std::string, T> >
{
public:
    static GAny from(const GAny &var)
    {
        if (var.is<std::map<std::string, T> >()) {
            return var;
        }

        if (!var.isObject()) {
            return GAny::undefined();
        }

        std::map<std::string, T> ret;

        const auto &objVar = var.as<GAnyObject>();
        {
            std::lock_guard locker(objVar.lock);
            for (const auto &v: objVar.var) {
                ret.insert(std::make_pair(v.first, v.second.castAs<T>()));
            }
        }

        return GAny::create(ret);
    }

    static GAny to(const std::map<std::string, T> &var)
    {
        return (std::shared_ptr<GAnyValue>) std::make_shared<GAnyObject>(
                std::unordered_map<std::string, GAny>(var.begin(), var.end()));
    }
};

template<typename T>
class caster<std::unordered_map<std::string, T> >
{
public:
    static GAny from(const GAny &var)
    {
        if (var.is<std::unordered_map<std::string, T> >()) {
            return var;
        }

        if (!var.isObject()) {
            return GAny::undefined();
        }

        std::unordered_map<std::string, T> ret;

        const auto &objVar = var.as<GAnyObject>();
        {
            std::lock_guard locker(objVar.lock);
            for (const auto &v: objVar.var) {
                ret.insert(std::make_pair(v.first, v.second.castAs<T>()));
            }
        }

        return GAny::create(ret);
    }

    static GAny to(const std::unordered_map<std::string, T> &var)
    {
        return (std::shared_ptr<GAnyValue>) std::make_shared<GAnyObject>(
                std::unordered_map<std::string, GAny>(var.begin(), var.end()));
    }
};

template<typename T>
class caster<std::shared_ptr<T> >
{
    using H = std::shared_ptr<T>;
public:
    static GAny from(const GAny &var)
    {
        if (var.is<H>()) {
            return var;
        }
        if (var.isNull()) {
            return GAny::create(H());
        }

        return GAny::undefined();
    }

    static GAny to(const std::shared_ptr<T> &var)
    {
        if (!var) {
            return GAny::null();
        }
        return GAny::create(var);
    }
};

template<>
class caster<std::nullptr_t>
{
public:
    static GAny from(const GAny &var)
    {
        return var;
    }

    static GAny to(std::nullptr_t v)
    {
        return GAny::null();
    }
};

template<int sz>
class caster<char[sz]>
{
public:
    static GAny to(const char *v)
    {
        return GAny(std::string(v));
    }
};

template<>
class caster<const char *>
{
public:
    static GAny from(const GAny &var)
    {
        if (var.is<std::string>()) {
            return GAny::create(var.as<std::string>().c_str());
        }
        return GAny();
    }

    static GAny to(const char *v)
    {
        return std::string(v);
    }
};

template<>
class caster<long>
{
public:
    static GAny from(const GAny &var)
    {
        return GAny::create((long) var.toInt64());
    }

    static GAny to(long v)
    {
        return GAny::create((int64_t) v);
    }
};

template<>
class caster<unsigned long>
{
public:
    static GAny from(const GAny &var)
    {
        return GAny::create((unsigned long) var.toInt64());
    }

    static GAny to(unsigned long v)
    {
        return GAny::create((uint64_t) v);
    }
};

template<>
class caster<long long>
{
public:
    static GAny from(const GAny &var)
    {
        return GAny::create((long long) var.toInt64());
    }

    static GAny to(long long v)
    {
        return GAny::create((int64_t) v);
    }
};

template<>
class caster<unsigned long long>
{
public:
    static GAny from(const GAny &var)
    {
        return GAny::create((unsigned long long) var.toInt64());
    }

    static GAny to(unsigned long long v)
    {
        return GAny::create((uint64_t) v);
    }
};

/// ================ GAny ================

template<typename T>
GAny::GAny(const T &var)
        :GAny(caster<T>::to(var))
{}

template<typename T>
GAny::GAny(detail::enable_if_t<!std::is_same<T, GAny>::value, T> &&var)
        :GAny(caster<T>::to(std::move(var)))
{}

template<typename T>
GAny::GAny(std::unique_ptr<T> &&v)
        : mVal(new GAnyValueP<std::unique_ptr<T>>(std::move(v)))
{}

template<typename Return, typename... Args>
GAny::GAny(Return (*f)(Args...), const std::string &doc)
        :GAny(GAnyFunction(f, doc))
{}

template<typename Return, typename Class, typename... Args>
GAny::GAny(Return (Class::*f)(Args...), const std::string &doc)
        :GAny(GAnyFunction(f, doc))
{}

template<typename Return, typename Class, typename... Args>
GAny::GAny(Return (Class::*f)(Args...) const, const std::string &doc)
        :GAny(GAnyFunction(f, doc))
{}

template<class T>
GAny GAny::create(const T &t)
{
    static_assert(!std::is_same<T, GAny>::value, "This should not happen.");
    return (std::shared_ptr<GAnyValue>) std::make_shared<GAnyValueP<T>>(t);
}

template<class T>
GAny GAny::create(T &&t)
{
    static_assert(!std::is_same<T, GAny>::value, "This should not happen.");
    return (std::shared_ptr<GAnyValue>) std::make_shared<GAnyValueP<typename std::remove_reference<T>::type>>(
            std::forward<T>(t));
}

template<typename T>
bool GAny::is() const
{
    return mVal->as(typeid(T)) != nullptr;
}

template<>
inline bool GAny::is<GAny>() const
{
    return true;
}

template<typename T>
const T &GAny::as() const
{
    T *ptr = (T *) mVal->as(typeid(T));
    if (!ptr) {
        throw GAnyException("Can not treat " + classTypeName() + " as " + GAnyClass::Class < T > ()->getName());
    }
    return *ptr;
}

template<>
inline const GAny &GAny::as<GAny>() const
{
    return *this;
}

template<typename T>
T &GAny::as()
{
    T *ptr = (T *) mVal->as(typeid(T));
    if (!ptr) {
        throw GAnyException("Can not treat " + classTypeName() + " as " + GAnyClass::Class < T > ()->getName());
    }
    return *ptr;
}

template<>
inline GAny &GAny::as<GAny>()
{
    return *this;
}

template<typename T>
T &GAny::unsafeAs()
{
    return ((GAnyValueP<T> *) mVal.get())->var;
}

template<typename T>
const T &GAny::unsafeAs() const
{
    return ((GAnyValueP<T> *) mVal.get())->var;
}

template<typename T>
detail::enable_if_t<!std::is_reference<T>::value && !std::is_pointer<T>::value, T>
GAny::castAs() const
{
    T *ptr = (T *) mVal->as(typeid(T));
    if (ptr) {
        return *ptr;
    }

    GAny ret = classObject().castToBase(*GAnyClass::instance<T>(), *this);
    if (ret.isUndefined()) {
        ret = caster<T>::from(*this);
    }
    if (!ret.template is<T>()) {
        throw GAnyException("Unable cast " + classTypeName() + " to " + GAnyClass::Class < T > ()->getName());
    }
    return ret.as<T>();
}

template<typename T>
detail::enable_if_t<std::is_reference<T>::value, T &>
GAny::castAs()
{
    typedef typename std::remove_const<typename std::remove_reference<T>::type>::type RmConstRefType;
    if (!is<RmConstRefType>()) {
        return *castAs<RmConstRefType *>();
    }

    return as<RmConstRefType>();
}

template<typename T>
detail::enable_if_t<std::is_pointer<T>::value, T> GAny::castAs()
{
    typedef typename std::remove_const<typename std::remove_pointer<T>::type>::type *RmConstPtr;
    typedef typename std::remove_pointer<T>::type RmPtr;

    // T* -> T*
    const void *ptr = mVal->as(typeid(T));
    if (ptr) {
        return *(T *) ptr;
    }

    // T -> T*
    ptr = mVal->as(typeid(RmPtr));
    if (ptr) {
        return (RmPtr *) ptr;
    }

    // T* -> const T *
    ptr = mVal->as(typeid(RmConstPtr));
    if (ptr) {
        return *(RmConstPtr *) ptr;
    }

    // nullptr
    if (isNull()) {
        return (T) nullptr;
    }

    GAny ret = classObject().castToBase(*GAnyClass::instance<RmPtr>(), *this);
    if (ret.isUndefined()) {
        ret = caster<T>::from(*this);
    }

    if (ret.template is<T>()) {
        return ret.template as<T>();
    }

    if (ret.is<RmConstPtr>()) {
        return ret.template as<RmConstPtr>();
    }

    throw GAnyException("Unable cast " + classTypeName() + " to " + GAnyClass::Class < T > ()->getName());
}

template<>
inline GAny GAny::castAs<GAny>() const
{
    return *this;
}

template<typename T>
T GAny::get(const std::string &name, T def)
{
    GAny ret = getItem(name);
    if (ret.isUndefined()) {
        return def;
    }
    return ret.as<T>();
}

template<typename T>
void GAny::set(const std::string &name, const T &v)
{
    setItem(name, v);
}

template<typename... Args>
GAny GAny::call(const std::string &function, Args &&... args) const
{
    if (isClass()) {
        // static method
        return as<GAnyClass>().call(GAny(), function, std::forward<Args>(args)...);
    } else if (!isUserObject() && isObject()) {
        auto func = getItem(function);
        return func(std::forward<Args>(args)...);
    }
    return classObject().call(*this, function, std::forward<Args>(args)...);
}

template<typename... Args>
GAny GAny::operator()(Args... args) const
{
    std::initializer_list<GAny> argv = {
            (GAny(std::move(args)))...
    };

    auto tArgc = (int32_t) argv.size();
    const GAny **tArgs = (const GAny **) alloca(sizeof(GAny *) * tArgc);
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &argv.begin()[i];
    }

    return _call(tArgs, tArgc);
}


inline GAny::GAny(const std::initializer_list<GAny> &init)
        : GAny()
{
    bool is_an_object = std::all_of(
            init.begin(), init.end(),
            [](const GAny &ele) {
                return ele.isArray() && ele.length() == 2 && ele.getItem(0).is<std::string>();
            });

    if (is_an_object) {
        std::map<std::string, GAny> obj;
        for (const GAny &p: init) {
            obj.insert(std::make_pair(p.getItem(0).as<std::string>(), p.getItem(1)));
        }
        *this = GAny(obj);
        return;
    }

    *this = GAny(std::vector<GAny>(init));
}

inline GAny GAny::object(const std::map<std::string, GAny> &m)
{
    return GAny(m);
}

inline GAny GAny::object(const std::unordered_map<std::string, GAny> &m)
{
    return GAny(m);
}

inline GAny GAny::array(const std::vector<GAny> &vec)
{
    return GAny(vec);
}

inline GAny GAny::array(const std::list<GAny> &lst)
{
    return GAny(lst);
}

inline GAny GAny::undefined()
{
    return std::make_shared<GAnyValue>();
}

inline GAny GAny::null()
{
    return create<std::nullptr_t>(nullptr);
}

inline const GAny GAny::environment()
{
    if (pfnGanyGetEnv) {
        return *reinterpret_cast<GAny *>(pfnGanyGetEnv());
    }
    return GAny::undefined();
}

inline const std::shared_ptr<GAnyValue> &GAny::value() const
{
    return mVal;
}

inline GAny GAny::clone() const
{
    return mVal->clone();
}

inline const std::string &GAny::classTypeName() const
{
    return classObject().getName();
}

inline const std::string &GAny::typeName() const
{
    return anyTypeNames()[(size_t) type()];
}

inline CppType GAny::cppType() const
{
    return classObject().mCppType;
}

inline AnyType GAny::type() const
{
    return classObject().mType;
}

inline GAnyClass &GAny::classObject() const
{
    return *mVal->classObject();
}

inline size_t GAny::length() const
{
    if (isUndefined() || isNull()) {
        return 0;
    }
    if (isArray()) {
        return as<GAnyArray>().length();
    }
    if (type() == AnyType::object_t) {
        return as<GAnyObject>().length();
    }
    auto &cl = classObject();
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::Length]);
    return ret.toInt64();
}

inline bool GAny::is(const CppType &cppType) const
{
    return mVal->as(cppType.typeIndex()) != nullptr;
}

inline bool GAny::is(const std::string &typeStr) const
{
    auto &classObj = classObject();
    if (typeStr.find('.') != std::string::npos) {
        return classObj.getNameSpace() + "." + classObj.getName() == typeStr;
    }
    return classObj.getName() == typeStr;
}

inline bool GAny::isUndefined() const
{
    return type() == AnyType::undefined_t;
}

inline bool GAny::isNull() const
{
    return type() == AnyType::null_t || mVal == nullptr || mVal->ptr() == nullptr;
}

inline bool GAny::isFunction() const
{
    return type() == AnyType::function_t;
}

inline bool GAny::isClass() const
{
    return type() == AnyType::class_t;
}

inline bool GAny::isException() const
{
    return is<GAnyException>();
}

inline bool GAny::isProperty() const
{
    return type() == AnyType::property_t;
}

inline bool GAny::isEnum() const
{
    return type() == AnyType::enum_t;
}

inline bool GAny::isObject() const
{
    return is<GAnyObject>();
}

inline bool GAny::isArray() const
{
    return type() == AnyType::array_t;
}

inline bool GAny::isInt8() const
{
    return type() == AnyType::int8_t;
}

inline bool GAny::isInt16() const
{
    return type() == AnyType::int16_t;
}

inline bool GAny::isInt32() const
{
    return type() == AnyType::int32_t;
}

inline bool GAny::isInt64() const
{
    return type() == AnyType::int64_t;
}

inline bool GAny::isFloat() const
{
    return type() == AnyType::float_t;
}

inline bool GAny::isDouble() const
{
    return type() == AnyType::double_t;
}

inline bool GAny::isNumber() const
{
    switch (type()) {
        case AnyType::int8_t:
        case AnyType::int16_t:
        case AnyType::int32_t:
        case AnyType::int64_t:
        case AnyType::float_t:
        case AnyType::double_t:
            return true;
        default:
            break;
    }
    return false;
}

inline bool GAny::isString() const
{
    return type() == AnyType::string_t;
}

inline bool GAny::isBoolean() const
{
    return type() == AnyType::boolean_t;
}

inline bool GAny::isUserObject() const
{
    return type() == AnyType::user_obj_t;
}

inline bool GAny::isCaller() const
{
    return type() == AnyType::caller_t;
}

inline const void *GAny::getPointer() const
{
    return mVal->ptr();
}

inline void *GAny::getPointer(const std::string &name, void *def)
{
    return get<void *>(name, def);
}

inline bool GAny::contains(const GAny &id) const
{
    if (id.isUndefined() || id.isNull()) {
        return false;
    }

    if (isObject()) {
        if (!id.isString()) {
            return false;
        }
        auto &obj = as<GAnyObject>();
        return obj.contains(id.toString());
    }
    if (isArray()) {
        auto &arr = as<GAnyArray>();
        return arr.contains(id);
    }
    if (classObject().containsMember("contains")) {
        try {
            return call("contains", id).toBool();
        } catch (GAnyException &) {}
    }
    if (isClass() && id.isString()) {
        return as<GAnyClass>().containsMember(id.toString());
    }
    return false;
}

inline void GAny::erase(const GAny &id)
{
    if (isObject()) {
        if (!id.isString()) {
            return;
        }
        auto &obj = as<GAnyObject>();
        obj.erase(id.castAs<std::string>());
        return;
    }
    if (isArray()) {
        auto &arr = as<GAnyArray>();
        arr.erase(id.toInt32());
        return;
    }
}

inline void GAny::pushBack(const GAny &rh)
{
    if (isArray()) {
        as<GAnyArray>().push_back(rh);
    }
}

inline void GAny::clear()
{
    if (isObject()) {
        auto &obj = as<GAnyObject>();
        obj.clear();
        return;
    }
    if (isArray()) {
        auto &arr = as<GAnyArray>();
        arr.clear();
        return;
    }
}

inline GAny GAny::iterator() const
{
    if (classObject().containsMember("iterator")) {
        return call("iterator");
    }
    return GAny::undefined();
}

inline bool GAny::hasNext() const
{
    return call("hasNext").toBool();
}

inline GAnyIteratorItem GAny::next() const
{
    return call("next").as<GAnyIteratorItem>();
}

inline GAny *GAny::overload(GAny func)
{
    if (!isFunction() || !func.isFunction()) {
        return nullptr;
    }

    // function with unknown parameters, overload is not supported
    if (!this->as<GAnyFunction>().mDoCheckArgs || !func.as<GAnyFunction>().mDoCheckArgs) {
        return nullptr;
    }

    auto &rFunc = func.as<GAnyFunction>();

    GAny *dest = this;
    while (dest->isFunction()) {
        if (dest->as<GAnyFunction>().compareArgs(rFunc)) {
            break;
        }
        dest = &dest->as<GAnyFunction>().mNext;
    }
    if (dest->isFunction()) {
        // overwrite
        GAny oldNext = dest->as<GAnyFunction>().mNext;
        *dest = func;
        dest->as<GAnyFunction>().mNext = oldNext;
    } else {
        // overload
        *dest = func;
        dest->as<GAnyFunction>().mNext = GAny();
    }
    return dest;
}

inline GAny GAny::_call(const std::string &function, const GAny **args, int32_t argc) const
{
    if (isClass()) {
        // static method
        return as<GAnyClass>()._call(GAny(), function, args, argc);
    } else if (!isUserObject() && isObject()) {
        auto func = getItem(function);
        return func._call(args, argc);
    }
    return classObject()._call(*this, function, args, argc);
}

inline GAny GAny::_call(const std::string &function, std::vector<GAny> &args) const
{
    auto tArgc = (int32_t) args.size();
    const GAny **tArgs = (const GAny **) alloca(sizeof(GAny *) * tArgc);
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _call(function, tArgs, tArgc);
}

inline GAny GAny::_call(MetaFunction metaFunc, const GAny **args, int32_t argc) const
{
    return _call(metaFunctionNames()[(size_t) metaFunc], args, argc);
}

inline GAny GAny::_call(MetaFunction metaFunc, std::vector<GAny> &args) const
{
    auto tArgc = (int32_t) args.size();
    const GAny **tArgs = (const GAny **) alloca(sizeof(GAny *) * tArgc);
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _call(metaFunc, tArgs, tArgc);
}

inline GAny GAny::_call(const GAny **args, int32_t argc) const
{
    if (isFunction()) {
        return as<GAnyFunction>()._call(args, argc);
    } else if (isClass()) {
        // constructor
        const auto &cls = as<GAnyClass>();
        if (!cls.fInit.isFunction()) {
            throw GAnyException("Class " + cls.mName + " does not have MetaFunc::Init function.");
        }
        return cls.fInit.as<GAnyFunction>()._call(args, argc);
    } else if (isCaller()) {
        return as<GAnyCaller>().call(args, argc);
    }
    throw GAnyException(classTypeName() + " can't be called as a function or constructor.");
    return GAny::undefined();
}

inline GAny GAny::_call(std::vector<GAny> &args) const
{
    auto tArgc = (int32_t) args.size();
    const GAny **tArgs = (const GAny **) alloca(sizeof(GAny *) * tArgc);
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _call(tArgs, tArgc);
}

inline GAny GAny::operator-() const
{
    if (this->isNumber()) {
        if (this->isDouble()) {
            return -this->toDouble();
        }
        if (this->isFloat()) {
            return -this->toFloat();
        }
        if (this->isInt64()) {
            return -this->toInt64();
        }
        if (this->isInt32()) {
            return -this->toInt32();
        }
        if (this->isInt16()) {
            return -this->toInt16();
        }
        if (this->isInt8()) {
            return -this->toInt8();
        }
    }

    auto &cl = classObject();
    auto ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::Negate]);
    if (ret.isUndefined()) {
        throw GAnyException(
                cl.mName + " operator " + metaFunctionNames()[(size_t) MetaFunction::Negate] + " returned Undefined.");
    }
    return ret;
}

#define DEF_OPERATOR_IMPL(METAF) \
    auto& cl = classObject(); \
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) METAF], rh); \
    if(ret.isUndefined()) throw GAnyException(cl.mName+" operator "+metaFunctionNames()[(size_t) METAF]+" with rh: "+rh.classTypeName()+"returned Undefined.");\
    return ret

#define DEF_FOUR_OPERATOR_IMPL(OP, METAF) \
    if (this->isNumber() && rh.isNumber()) { \
        if (this->isDouble() || rh.isDouble()) { return this->toDouble() OP rh.toDouble(); } \
        if (this->isFloat() || rh.isFloat()) { return this->toFloat() OP rh.toFloat(); } \
        if (this->isInt64() || rh.isInt64()) { \
            if (this->is<uint64_t>() || rh.is<uint64_t>()) { \
                return (uint64_t)(this->toInt64()) OP (uint64_t)(rh.toInt64()); \
            } \
            return this->toInt64() OP rh.toInt64(); \
        } \
        if (this->isInt32() || rh.isInt32()) { \
            if (this->is<uint32_t>() || rh.is<uint32_t>()) { \
                return (uint32_t)(this->toInt32()) OP (uint32_t)(rh.toInt32()); \
            } \
            return this->toInt32() OP rh.toInt32(); \
        } \
        if (this->isInt16() || rh.isInt16()) { \
            if (this->is<uint16_t>() || rh.is<uint16_t>()) { \
                return (uint16_t)(this->toInt16()) OP (uint16_t)(rh.toInt16()); \
            } \
            return this->toInt16() OP rh.toInt16(); \
        }                                 \
        if (this->is<uint8_t>() || rh.is<uint8_t>()) { \
            return (uint8_t)(this->toInt8()) OP (uint8_t)(rh.toInt8()); \
        } \
        return this->toInt8() OP rh.toInt8(); \
    } \
    DEF_OPERATOR_IMPL(METAF)


inline GAny GAny::operator+(const GAny &rh) const
{
    DEF_FOUR_OPERATOR_IMPL(+, MetaFunction::Addition);
}

inline GAny GAny::operator-(const GAny &rh) const
{
    DEF_FOUR_OPERATOR_IMPL(-, MetaFunction::Subtraction);
}

inline GAny GAny::operator*(const GAny &rh) const
{
    DEF_FOUR_OPERATOR_IMPL(*, MetaFunction::Multiplication);
}

inline GAny GAny::operator/(const GAny &rh) const
{
    DEF_FOUR_OPERATOR_IMPL(/, MetaFunction::Division);
}

inline GAny GAny::operator%(const GAny &rh) const
{
    if (this->isNumber() && rh.isNumber()) {
        if (this->isDouble() || rh.isDouble()) {
            return std::fmod(this->toDouble(), rh.toDouble());
        }
        if (this->isFloat() || rh.isFloat()) {
            return std::fmod(this->toFloat(), rh.toFloat());
        }
        if (this->isInt64() || rh.isInt64()) {
            if (this->is<uint64_t>() || rh.is<uint64_t>()) {
                return (uint64_t) (this->toInt64()) % (uint64_t) (rh.toInt64());
            }
            return this->toInt64() % rh.toInt64();
        }
        if (this->isInt32() || rh.isInt32()) {
            if (this->is<uint32_t>() || rh.is<uint32_t>()) {
                return (uint32_t) (this->toInt32()) % (uint32_t) (rh.toInt32());
            }
            return this->toInt32() % rh.toInt32();
        }
        if (this->isInt16() || rh.isInt16()) {
            if (this->is<uint16_t>() || rh.is<uint16_t>()) {
                return (uint16_t) (this->toInt16()) % (uint16_t) (rh.toInt16());
            }
            return this->toInt16() % rh.toInt16();
        }
        if (this->is<uint8_t>() || rh.is<uint8_t>()) {
            return (uint8_t) (this->toInt8()) % (uint8_t) (rh.toInt8());
        }
        return this->toInt8() % rh.toInt8();
    }
    DEF_OPERATOR_IMPL(MetaFunction::Modulo);
}

inline GAny GAny::operator^(const GAny &rh) const
{
    if (this->isNumber() && rh.isNumber()) {
        if (this->isInt64() || rh.isInt64()) {
            if (this->is<uint64_t>() || rh.is<uint64_t>()) {
                return (uint64_t) (this->toInt64()) ^ (uint64_t) (rh.toInt64());
            }
            return this->toInt64() ^ rh.toInt64();
        }
        if (this->isInt32() || rh.isInt32()) {
            if (this->is<uint32_t>() || rh.is<uint32_t>()) {
                return (uint32_t) (this->toInt32()) ^ (uint32_t) (rh.toInt32());
            }
            return this->toInt32() ^ rh.toInt32();
        }
        if (this->isInt16() || rh.isInt16()) {
            if (this->is<uint16_t>() || rh.is<uint16_t>()) {
                return (uint16_t) (this->toInt16()) ^ (uint16_t) (rh.toInt16());
            }
            return this->toInt16() ^ rh.toInt16();
        }
        if (this->isInt8() || rh.isInt8()) {
            if (this->is<uint8_t>() || rh.is<uint8_t>()) {
                return (uint8_t) (this->toInt8()) ^ (uint8_t) (rh.toInt8());
            }
            return this->toInt8() ^ rh.toInt8();
        }
    }
    DEF_OPERATOR_IMPL(MetaFunction::BitXor);
}

inline GAny GAny::operator|(const GAny &rh) const
{
    if (this->isNumber() && rh.isNumber()) {
        if (this->isInt64() || rh.isInt64()) {
            if (this->is<uint64_t>() || rh.is<uint64_t>()) {
                return (uint64_t) (this->toInt64() | rh.toInt64());
            }
            return this->toInt64() | rh.toInt64();
        }
        if (this->isInt32() || rh.isInt32()) {
            if (this->is<uint32_t>() || rh.is<uint32_t>()) {
                return (uint32_t) (this->toInt32() | rh.toInt32());
            }
            return this->toInt32() | rh.toInt32();
        }
        if (this->isInt16() || rh.isInt16()) {
            if (this->is<uint16_t>() || rh.is<uint16_t>()) {
                return (uint16_t) (this->toInt16()) | (uint16_t) (rh.toInt16());
            }
            return this->toInt16() | rh.toInt16();
        }
        if (this->isInt8() || rh.isInt8()) {
            if (this->is<uint8_t>() || rh.is<uint8_t>()) {
                return (uint8_t) (this->toInt8() | rh.toInt8());
            }
            return this->toInt8() | rh.toInt8();
        }
    }
    DEF_OPERATOR_IMPL(MetaFunction::BitOr);
}

inline GAny GAny::operator&(const GAny &rh) const
{
    if (this->isNumber() && rh.isNumber()) {
        if (this->isInt64() || rh.isInt64()) {
            if (this->is<uint64_t>() || rh.is<uint64_t>()) {
                return (uint64_t) (this->toInt64() & rh.toInt64());
            }
            return this->toInt64() & rh.toInt64();
        }
        if (this->isInt32() || rh.isInt32()) {
            if (this->is<uint32_t>() || rh.is<uint32_t>()) {
                return (uint32_t) (this->toInt32() & rh.toInt32());
            }
            return this->toInt32() & rh.toInt32();
        }
        if (this->isInt16() || rh.isInt16()) {
            if (this->is<uint16_t>() || rh.is<uint16_t>()) {
                return (uint16_t) (this->toInt16() & rh.toInt16());
            }
            return this->toInt16() & rh.toInt16();
        }
        if (this->isInt8() || rh.isInt8()) {
            if (this->is<uint8_t>() || rh.is<uint8_t>()) {
                return (uint8_t) (this->toInt8() & rh.toInt8());
            }
            return this->toInt8() & rh.toInt8();
        }
    }
    DEF_OPERATOR_IMPL(MetaFunction::BitAnd);
}

#undef DEF_OPERATOR_IMPL
#undef DEF_FOUR_OPERATOR_IMPL

inline GAny GAny::operator~() const
{
    if (this->isNumber()) {
        if (this->isInt64()) {
            if (this->is<uint64_t>()) {
                return ~this->as<uint64_t>();
            }
            return ~this->toInt64();
        }
        if (this->isInt32()) {
            if (this->is<uint32_t>()) {
                return ~this->as<uint32_t>();
            }
            return ~this->toInt32();
        }
        if (this->isInt16()) {
            if (this->is<uint16_t>()) {
                return ~this->as<uint16_t>();
            }
            return ~this->toInt16();
        }
        if (this->isInt8()) {
            if (this->is<uint8_t>()) {
                return ~this->as<uint8_t>();
            }
            return ~this->toInt8();
        }
    }

    auto &cl = classObject();
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::BitNot]);
    if (ret.isUndefined()) {
        throw GAnyException(
                cl.mName + " operator " + metaFunctionNames()[(size_t) MetaFunction::BitNot] + " returned Undefined.");
    }
    return ret;
}

inline bool GAny::operator==(const GAny &rh) const
{
    if (this->isNumber() && rh.isNumber()) {
        if (this->isDouble() || rh.isDouble() || this->isFloat() || rh.isFloat()) {
            return std::abs(this->toDouble() - rh.toDouble()) < 1e-6;
        }
        if (this->isInt64() || rh.isInt64()) {
            return this->toInt64() == rh.toInt64();
        }
        if (this->isInt32() || rh.isInt32()) {
            return this->toInt32() == rh.toInt32();
        }
        if (this->isInt16() || rh.isInt16()) {
            return this->toInt16() == rh.toInt16();
        }
        return this->toInt8() == rh.toInt8();
    }

    auto &cl = classObject();
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::EqualTo], rh);
    if (!ret.is<bool>()) {
        throw GAnyException(cl.mName + " operator " + metaFunctionNames()[(size_t) MetaFunction::EqualTo] +
                            " with rh: " + rh.classTypeName() + " returned " + ret.classTypeName() +
                            " instead of bool.");
    }
    return ret.as<bool>();
}

inline bool GAny::operator<(const GAny &rh) const
{
    if (this->isNumber() && rh.isNumber()) {
        if (this->isDouble() || rh.isDouble()) {
            return this->toDouble() < rh.toDouble();
        }
        if (this->isFloat() || rh.isFloat()) {
            return this->toFloat() < rh.toFloat();
        }
        if (this->isInt64() || rh.isInt64()) {
            if (this->is<uint64_t>() || rh.is<uint64_t>()) {
                return (uint64_t) (this->toInt64()) < (uint64_t) (rh.toInt64());
            }
            return this->toInt64() < rh.toInt64();
        }
        if (this->isInt32() || rh.isInt32()) {
            if (this->is<uint32_t>() || rh.is<uint32_t>()) {
                return (uint32_t) (this->toInt32()) < (uint32_t) (rh.toInt32());
            }
            return this->toInt32() < rh.toInt32();
        }
        if (this->isInt16() || rh.isInt16()) {
            if (this->is<uint16_t>() || rh.is<uint16_t>()) {
                return (uint16_t) (this->toInt16()) < (uint16_t) (rh.toInt16());
            }
            return this->toInt16() < rh.toInt16();
        }
        if (this->is<uint8_t>() || rh.is<uint8_t>()) {
            return (uint8_t) (this->toInt8()) < (uint8_t) (rh.toInt8());
        }
        return this->toInt8() < rh.toInt8();
    }

    auto &cl = classObject();
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::LessThan], rh);
    if (!ret.is<bool>()) {
        throw GAnyException(cl.mName + " operator " + metaFunctionNames()[(size_t) MetaFunction::LessThan] +
                            " with rh: " + rh.classTypeName() + " returned " + ret.classTypeName() +
                            " instead of bool.");
    }
    return ret.as<bool>();
}

inline bool GAny::operator==(std::nullptr_t) const
{
    return isUndefined() || isNull();
}

inline bool GAny::operator!=(std::nullptr_t) const
{
    return !(*this == nullptr);
}

inline GAny GAny::getItem(const GAny &i) const
{
    if (isUndefined()) {
        return GAny::undefined();
    }

    if (i.isString() && !isArray()) {
        GString si(i.castAs<std::string>());
        if (si.indexOf(".") > 0) {
            auto paths = si.split(".");
            if (!paths.empty()) {
                GAny ret = *this;
                for (const auto &path: paths) {
                    std::string p = path.toStdString();
                    if (p.empty()) {
                        continue;
                    }
                    ret = ret.getItem(p);
                }
                return ret;
            }
            return GAny::undefined();
        }
    }

    if (isObject()) {
        if (!i.isString()) {
            return GAny::undefined();
        }
        auto &obj = as<GAnyObject>();
        const auto &key = i.castAs<std::string>();
        GAny v = obj[key];

        if (v.isUndefined()) {
            try {
                v = classObject().getItem((*this), i);
            } catch (GAnyException &e) {
                if (this->isClass() || this->isUserObject()) {
                    throw e;
                }
                v = GAny::undefined();
            }
        }
        return v;
    }
    if (isArray()) {
        if (i.isString()) {
            try {
                return classObject().getItem((*this), i);
            } catch (GAnyException &) {
                return GAny::undefined();
            }
        }
        auto &arr = as<GAnyArray>();
        return arr[i.toInt32()];
    }
    if (isEnum()) {
        if (!i.isString()) {
            return GAny::undefined();
        }
        return as<GAnyClass::GAnyEnum>().enumObj.getItem(i);
    }
    if (isClass() && i.isString()) {
        return as<GAnyClass>().findMember(i.toString());
    }

    return classObject().getItem((*this), i);
}

inline void GAny::setItem(const GAny &i, const GAny &v)
{
    if (isUndefined()) {
        *this = object();
    }

    if (isObject()) {
        if (!i.isString()) {
            return;
        }
        std::string key = i.castAs<std::string>();
        if (isClass() || isUserObject()) {
            GAny attr = classObject().findMember(key);
            if (attr.isProperty()) {
                classObject().setItem((*this), i, v);
                return;
            }
        }
        auto &obj = as<GAnyObject>();
        obj.set(key, v);
        return;
    }
    if (isArray()) {
        auto &arr = as<GAnyArray>();
        arr.set(i.toInt32(), v);
        return;
    }
    if (isEnum()) {
        as<GAnyClass::GAnyEnum>().enumObj.setItem(i, v);
        return;
    }

    classObject().setItem((*this), i, v);
}

inline void GAny::delItem(const GAny &i)
{
    if (isUndefined()) {
        return;
    }

    if (isObject()) {
        if (!i.isString()) {
            throw GAnyException("DelItem object expects a string as index.");
        }
        auto &obj = as<GAnyObject>();
        obj.erase(i.castAs<std::string>());
        return;
    }
    if (isArray()) {
        auto &arr = as<GAnyArray>();
        arr.erase(i.toInt32());
        return;
    }

    auto &cl = classObject();
    cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::DelItem], i);
}

inline const GAny GAny::operator[](const GAny &key) const
{
    return getItem(key);
}

inline GAny &GAny::operator[](const GAny &key)
{
    if (isUndefined()) {
        *this = object();
    }

    if (isObject()) {
        if (!key.isString()) {
            throw GAnyException("GAny::operator[] object expects a string as index.");
        }
        auto &obj = as<GAnyObject>();
        const auto &keyS = key.as<std::string>();
        std::lock_guard locker(obj.lock);
        auto it = obj.var.find(keyS);
        if (it != obj.var.end()) {
            return it->second;
        }
        auto ret = obj.var.insert(std::make_pair(keyS, GAny()));
        return ret.first->second;
    }
    if (isArray()) {
        auto &arr = as<GAnyArray>();
        std::lock_guard locker(arr.lock);
        return arr.var[key.toInt32()];
    }
    if (isEnum()) {
        return as<GAnyClass::GAnyEnum>().enumObj[key];
    }

    throw GAnyException(classTypeName() + ": Operator [] can't be used as a lvalue.");
    return *this;
}

inline std::string GAny::toString() const
{
    switch (type()) {
        case AnyType::undefined_t: {
            return "undefined";
        }
        case AnyType::null_t: {
            return "null";
        }
        case AnyType::boolean_t: {
            return castAs<bool>() ? "true" : "false";
        }
        case AnyType::int8_t: {
            std::stringstream ss;
            if (is<uint8_t>()) {
                ss << castAs<uint8_t>();
            } else if (is<int8_t>()) {
                ss << castAs<int8_t>();
            } else if (is<char>()) {
                ss << castAs<char>();
            }
            return ss.str();
        }
        case AnyType::int16_t: {
            std::stringstream ss;
            if (is<uint16_t>()) {
                ss << castAs<uint16_t>();
            } else {
                ss << castAs<int16_t>();
            }
            return ss.str();
        }
        case AnyType::int32_t: {
            std::stringstream ss;
            if (is<uint32_t>()) {
                ss << castAs<uint32_t>();
            } else {
                ss << castAs<int32_t>();
            }
            return ss.str();
        }
        case AnyType::int64_t: {
            std::stringstream ss;
            if (is<uint64_t>()) {
                ss << castAs<uint64_t>();
            } else {
                ss << castAs<int64_t>();
            }
            return ss.str();
        }
        case AnyType::float_t: {
            std::stringstream ss;
            ss << std::setprecision(7) << castAs<float>();
            return ss.str();
        }
        case AnyType::double_t: {
            std::stringstream ss;
            ss << std::setprecision(15) << castAs<double>();
            return ss.str();
        }
        case AnyType::string_t: {
            return castAs<std::string>();
        }
        case AnyType::array_t:
        case AnyType::object_t:
        case AnyType::class_t: {
            return toJsonString();
        }
        default:
            break;
    }

    auto &cl = classObject();
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::ToString]);
    if (!ret.is<std::string>()) {
        throw GAnyException(classTypeName() + ": toString must return a string.");
    }
    return ret.as<std::string>();
}

inline int8_t GAny::toInt8() const
{
    switch (type()) {
        case AnyType::int8_t: {
            if (is<uint8_t>()) {
                return (int8_t) as<uint8_t>();
            } else if (is<int8_t>()) {
                return as<int8_t>();
            } else if (is<char>()) {
                return (int8_t) as<char>();
            }
        }
        default:
            break;
    }
    return (int8_t) toInt32();
}

inline int16_t GAny::toInt16() const
{
    switch (type()) {
        case AnyType::int8_t: {
            if (is<uint8_t>()) {
                return (int16_t) as<uint8_t>();
            } else if (is<int8_t>()) {
                return as<int8_t>();
            } else if (is<char>()) {
                return (int16_t) as<char>();
            }
        }
        case AnyType::int16_t: {
            if (is<uint16_t>()) {
                return (int16_t) castAs<uint16_t>();
            } else {
                return castAs<int16_t>();
            }
        }
        default:
            break;
    }
    return (int16_t) toInt32();
}

inline int32_t GAny::toInt32() const
{
    switch (type()) {
        case AnyType::undefined_t:
        case AnyType::null_t: {
            return 0;
        }
        case AnyType::boolean_t: {
            return castAs<bool>() ? 1 : 0;
        }
        case AnyType::int8_t: {
            if (is<uint8_t>()) {
                return (int32_t) castAs<uint8_t>();
            } else if (is<int8_t>()) {
                return (int32_t) castAs<int8_t>();
            } else if (is<char>()) {
                return (int32_t) castAs<char>();
            }
        }
        case AnyType::int16_t: {
            if (is<uint16_t>()) {
                return (int32_t) castAs<uint16_t>();
            } else {
                return (int32_t) castAs<int16_t>();
            }
        }
        case AnyType::int32_t: {
            if (is<uint32_t>()) {
                return (int32_t) castAs<uint32_t>();
            } else {
                return (int32_t) castAs<int32_t>();
            }
        }
        case AnyType::int64_t: {
            if (is<uint64_t>()) {
                return (int32_t) castAs<uint64_t>();
            } else {
                return (int32_t) castAs<int64_t>();
            }
        }
        case AnyType::float_t: {
            return (int32_t) castAs<float>();
        }
        case AnyType::double_t: {
            return (int32_t) castAs<double>();
        }
        case AnyType::string_t: {
            std::stringstream ss;
            ss << castAs<std::string>();
            int32_t v;
            ss >> v;
            return v;
        }
        default:
            break;
    }

    auto &cl = classObject();
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::ToInt32]);
    if (!ret.is<int32_t>()) {
        throw GAnyException(classTypeName() + ": toInt32 must return an int32.");
    }
    return ret.as<int32_t>();
}

inline int64_t GAny::toInt64() const
{
    switch (type()) {
        case AnyType::undefined_t:
        case AnyType::null_t: {
            return 0LL;
        }
        case AnyType::boolean_t: {
            return castAs<bool>() ? 1LL : 0LL;
        }
        case AnyType::int8_t: {
            if (is<uint8_t>()) {
                return (int64_t) castAs<uint8_t>();
            } else if (is<int8_t>()) {
                return (int64_t) castAs<int8_t>();
            } else if (is<char>()) {
                return (int64_t) castAs<char>();
            }
        }
        case AnyType::int16_t: {
            if (is<uint16_t>()) {
                return (int64_t) castAs<uint16_t>();
            } else {
                return (int64_t) castAs<int16_t>();
            }
        }
        case AnyType::int32_t: {
            if (is<uint32_t>()) {
                return (int64_t) castAs<uint32_t>();
            } else {
                return (int64_t) castAs<int32_t>();
            }
        }
        case AnyType::int64_t: {
            if (is<uint64_t>()) {
                return (int64_t) castAs<uint64_t>();
            } else {
                return castAs<int64_t>();
            }
        }
        case AnyType::float_t: {
            return (int64_t) castAs<float>();
        }
        case AnyType::double_t: {
            return (int64_t) castAs<double>();
        }
        case AnyType::string_t: {
            std::stringstream ss;
            ss << castAs<std::string>();
            int64_t v;
            ss >> v;
            return v;
        }
        default:
            break;
    }

    auto &cl = classObject();
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::ToInt64]);
    if (!ret.is<int64_t>()) {
        throw GAnyException(classTypeName() + ": toInt64 must return an int64.");
    }
    return ret.as<int64_t>();
}

inline float GAny::toFloat() const
{
    if (is<float>()) {
        return castAs<float>();
    }
    if (is<double>()) {
        return (float) castAs<double>();
    }

    return (float) toDouble();
}

inline double GAny::toDouble() const
{
    switch (type()) {
        case AnyType::undefined_t:
        case AnyType::null_t: {
            return 0;
        }
        case AnyType::boolean_t: {
            return castAs<bool>() ? 1 : 0;
        }
        case AnyType::int8_t: {
            if (is<uint8_t>()) {
                return (double) castAs<uint8_t>();
            } else if (is<int8_t>()) {
                return (double) castAs<int8_t>();
            } else if (is<char>()) {
                return (double) castAs<char>();
            }
        }
        case AnyType::int16_t: {
            if (is<uint16_t>()) {
                return (double) castAs<uint16_t>();
            } else {
                return (double) castAs<int16_t>();
            }
        }
        case AnyType::int32_t: {
            if (is<uint32_t>()) {
                return (double) castAs<uint32_t>();
            } else {
                return (double) castAs<int32_t>();
            }
        }
        case AnyType::int64_t: {
            if (is<uint64_t>()) {
                return (double) castAs<uint64_t>();
            } else {
                return (double) castAs<int64_t>();
            }
        }
        case AnyType::float_t: {
            return (double) castAs<float>();
        }
        case AnyType::double_t: {
            return (double) castAs<double>();
        }
        case AnyType::string_t: {
            std::stringstream ss;
            ss << castAs<std::string>();
            double v;
            ss >> v;
            return v;
        }
        default:
            break;
    }

    auto &cl = classObject();
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::ToDouble]);
    if (!ret.is<double>()) {
        throw GAnyException(classTypeName() + ": toDouble must return a double.");
    }
    return ret.as<double>();
}

inline bool GAny::toBool() const
{
    switch (type()) {
        case AnyType::undefined_t:
        case AnyType::null_t: {
            return false;
        }
        case AnyType::boolean_t: {
            return as<bool>();
        }
        case AnyType::int8_t: {
            if (is<uint8_t>()) {
                return as<uint8_t>() != 0;
            } else if (is<int8_t>()) {
                return castAs<int8_t>() != 0;
            } else if (is<char>()) {
                return castAs<char>() != 0;
            }
        };
        case AnyType::int32_t: {
            if (is<uint32_t>()) {
                return castAs<uint32_t>() != 0;
            } else {
                return castAs<int32_t>() != 0;
            }
        }
        case AnyType::int64_t: {
            if (is<uint64_t>()) {
                return castAs<uint64_t>() != 0ULL;
            } else {
                return castAs<int64_t>() != 0LL;
            }
        }
        case AnyType::float_t: {
            return castAs<float>() != 0.f;
        }
        case AnyType::double_t: {
            return castAs<double>() != 0;
        }
        case AnyType::string_t: {
            return !castAs<std::string>().empty();
        }
        default:
            break;
    }

    auto &cl = classObject();
    GAny ret = cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::ToBoolean]);
    if (!ret.is<bool>()) {
        throw GAnyException(classTypeName() + ": toBool must return a bool.");
    }
    return ret.as<bool>();
}

inline GAny GAny::toObject() const
{
    if (isUndefined() || isNull()) {
        return GAny::object();
    }
    if (isObject() || isArray()) {
        return *this;
    }

    auto &cl = classObject();
    return cl.call(*this, metaFunctionNames()[(size_t) MetaFunction::ToObject]);
}

inline std::ostream &GAny::dumpJson(std::ostream &o, const int indent, const int current_indent) const
{
    static auto dump_str = [](const std::string &value) -> std::string {
        std::string out;
        out.reserve(value.size() * 2);
        out += '"';
        for (size_t i = 0; i < value.length(); i++) {
            const char ch = value[i];
            uint8_t uch = ch;
            switch (uch) {
                case '\\':
                    out += "\\\\";
                    break;
                case '"':
                    out += "\\\"";
                    break;
                case '\b':
                    out += "\\b";
                    break;
                case '\f':
                    out += "\\f";
                    break;
                case '\n':
                    out += "\\n";
                    break;
                case '\r':
                    out += "\\r";
                    break;
                case '\t':
                    out += "\\t";
                    break;
                case 0xe2:
                    if (static_cast<uint8_t>(value[i + 1]) == 0x80
                        && static_cast<uint8_t>(value[i + 2]) == 0xa8) {
                        out += "\\u2028";
                        i += 2;
                    } else if (static_cast<uint8_t>(value[i + 1]) == 0x80
                               && static_cast<uint8_t>(value[i + 2]) == 0xa9) {
                        out += "\\u2029";
                        i += 2;
                    } else {
                        out.push_back(ch);
                    }
                    break;
                default:
                    if (uch <= 0x1f) {
                        char buf[8];
                        snprintf(buf, sizeof buf, "\\u%04x", ch);
                        out.append(buf, buf + 8);
                    } else {
                        out.push_back(ch);
                    }
                    break;
            }
        }
        out += '"';
        return out;
    };
    switch (classObject().mType) {
        case AnyType::undefined_t: {
            return o;
        }
        case AnyType::null_t: {
            return o << "null";
        }
        case AnyType::boolean_t: {
            return o << (castAs<bool>() ? "true" : "false");
        }
        case AnyType::int8_t : {
            if (is<uint8_t>()) {
                return o << castAs<uint8_t>();
            } else if (is<int8_t>()) {
                return o << castAs<int8_t>();
            } else if (is<char>()) {
                return o << castAs<char>();
            }
        }
        case AnyType::int16_t : {
            if (is<uint16_t>()) {
                return o << castAs<uint16_t>();
            } else {
                return o << castAs<int16_t>();
            }
        }
        case AnyType::int32_t: {
            if (is<uint32_t>()) {
                return o << castAs<uint32_t>();
            } else {
                return o << castAs<int32_t>();
            }
        }
        case AnyType::int64_t: {
            if (is<uint64_t>()) {
                return o << castAs<uint64_t>();
            } else {
                return o << castAs<int64_t>();
            }
        }
        case AnyType::float_t: {
            std::stringstream floats;
            floats << std::setprecision(7) << castAs<float>();
            return o << floats.str();
        }
        case AnyType::double_t: {
            std::stringstream floats;
            floats << std::setprecision(15) << castAs<double>();
            return o << floats.str();
        }
        case AnyType::string_t: {
            return o << dump_str(castAs<std::string>());
        }
        case AnyType::array_t: {
            const auto &vec = unsafeAs<std::vector<GAny>>();
            const auto N = vec.size();
            if (N == 0) {
                return o << "[]";
            }
            o << '[';
            std::string indent_front, ident_back("]");
            int new_indent = current_indent + indent;
            if (indent >= 0) {
                indent_front = std::string(new_indent + 1, ' ');
                indent_front[0] = '\n';
                ident_back = std::string(current_indent + 2, ' ');
                ident_back[0] = '\n';
                ident_back.back() = ']';
            }
            int index = 0;
            for (auto it = vec.begin(); it != vec.end(); ++it) {
                if (it->isUndefined()) {
                    continue;
                }
                if (index > 0) {
                    o << ",";
                }
                o << indent_front;
                it->dumpJson(o, indent, new_indent);
                index++;
            }
            o << ident_back;
            return o;
        }
        case AnyType::object_t: {
            const auto &obj = unsafeAs<std::unordered_map<std::string, GAny>>();
            const auto N = obj.size();
            if (N == 0) {
                return o << "{}";
            }
            o << '{';
            int new_indent = current_indent + indent;
            std::string indent_front, ident_back("}");
            if (indent >= 0) {
                indent_front = std::string(new_indent + 1, ' ');
                indent_front[0] = '\n';
                ident_back = std::string(current_indent + 2, ' ');
                ident_back[0] = '\n';
                ident_back.back() = '}';
            }
            int index = 0;
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                if (it->second.isUndefined()) {
                    continue;
                }
                if (index > 0) {
                    o << ",";
                }
                o << indent_front;
                o << dump_str(it->first);
                o << ":";
                if (indent > 0) {
                    o << " ";
                }
                it->second.dumpJson(o, indent, new_indent);
                index++;
            }
            o << ident_back;
            return o;
        }
        case AnyType::class_t: {
            auto &cl = as<GAnyClass>();
            return o << "\"<Class: " << cl.mName << ">\"";
        }
        default: {
            if (classObject().mType == AnyType::user_obj_t && classObject().containsMember(MetaFunction::ToObject)) {
                return toObject().dumpJson(o, indent, current_indent);
            }

            try {
                std::stringstream sst;
                sst << "\"" << toString() << "\"";
                return o << sst.str();
            } catch (GAnyException &) {
            }

            return o << "\"<" << classTypeName() << " at " << value().get() << ">\"";
        }
    }
}

inline std::string GAny::toJsonString(int indent) const
{
    std::stringstream sst;
    dumpJson(sst, indent);
    return sst.str();
}

inline GAny GAny::parseJson(const std::string &json)
{
    GAny obj = GAny::object();
    if (pfnGanyParseJson) {
        pfnGanyParseJson(json.c_str(), &obj);
    }
    return obj;
}

/// ================ GAnyFunction ================

inline GAnyFunction GAnyFunction::createVariadicFunction(const std::string &name, const std::string &doc,
                                                         std::function<GAny(const GAny **args, int32_t argc)> func)
{
    GAnyFunction anyFunc;
    anyFunc.mName = name;
    anyFunc.mDoc = doc;
    anyFunc.mFunc = std::move(func);
    anyFunc.mDoCheckArgs = false;

    return anyFunc;
}

inline GAny GAnyFunction::_call(const GAny **args, int32_t argc) const
{
    std::vector<const GAnyFunction *> unmatched;
    std::vector<std::pair<const GAnyFunction *, GAnyException>> catches;

    // [1] perfect match
    for (const GAnyFunction *overload = this; overload != nullptr; overload = &overload->mNext.as<GAnyFunction>()) {
        if (overload->matchingArgv(args, argc)) {
            try {
                return overload->mFunc(args, argc);
            }
            catch (GAnyException &e) {
                catches.emplace_back(overload, e);
            }
            break;
        } else if (!overload->mDoCheckArgs || (overload->mArgTypes.size() == argc + 1)) {
            unmatched.push_back(overload);
        }
        if (!overload->mNext.isFunction()) {
            overload = nullptr;
            break;
        }
    }

    // [2] fuzzy matching
    for (const GAnyFunction *overload: unmatched) {
        try {
            return overload->mFunc(args, argc);
        }
        catch (GAnyException &e) {
            catches.emplace_back(overload, e);
        }
    }


    std::stringstream stream;
    stream << "Failed to call " << (mIsMethod ? "method" : "function") << " with input arguments: [";

    for (int32_t i = 0; i < argc; i++) {
        stream << (i == 0 ? "" : ",") << args[i]->classTypeName();
    }
    stream << "].\nAt: " << (*this);

    if (!catches.empty()) {
        stream << "\n" << "Caused by:";

        for (auto it = catches.begin(); it != catches.end(); it++) {
            stream << "\n";
            stream << "Attempt to call: \"" << it->first->signature()
                   << "\" failed, Exception:\n";

            std::stringstream nextExcept;
            nextExcept << it->second.what();
            std::string line;
            int32_t lineC = 0;
            while (std::getline(nextExcept, line)) {
                if (lineC > 0) {
                    stream << "\n";
                }
                lineC++;
                stream << "  " << line;
            }
        }
    }

    throw GAnyException(stream.str());
    return GAny::undefined();
}

inline std::string GAnyFunction::signature() const
{
    std::stringstream sst;

    if (!mDoCheckArgs) {
        sst << (mName.empty() ? "function" : mName) << "(...)";
        return sst.str();
    }
    sst << (mName.empty() ? "function" : mName) << "(";
    for (size_t i = 1; i < mArgTypes.size(); i++) {
        if (mIsMethod && i == 1 && mName.substr(0, 6) != metaFunctionNames()[(size_t) MetaFunction::Init]) {
            sst << "self" << (i + 1 == mArgTypes.size() ? "" : ", ");
        } else {
            sst << "arg" << i - 1 << ": ";
            sst << mArgTypes[i].as<GAnyClass>().getName()
                << (i + 1 == mArgTypes.size() ? "" : ", ");
        }
    }

    sst << ") -> ";
    sst << mArgTypes[0].as<GAnyClass>().getName();

    return sst.str();
}

inline bool GAnyFunction::compareArgs(const GAnyFunction &rFunc) const
{
    if (!mDoCheckArgs || !rFunc.mDoCheckArgs) {
        return false;
    }
    if (mArgTypes.size() != rFunc.mArgTypes.size()) {
        return false;
    }
    for (size_t i = 1; i < mArgTypes.size(); i++) {
        if (mArgTypes[i].as<GAnyClass>() != rFunc.mArgTypes[i].as<GAnyClass>()) {
            return false;
        }
    }
    return true;
}

inline bool GAnyFunction::matchingArgv(const GAny **args, int32_t argc) const
{
    if (!mDoCheckArgs) {
        return true;
    }
    if (mArgTypes.size() != argc + 1) {
        return false;
    }
    for (size_t i = 1; i < mArgTypes.size(); i++) {
        const auto &lClazz = mArgTypes[i].as<GAnyClass>();
        if (lClazz.mCppType.isClassGAny()) {
            continue;
        }
        if (lClazz != args[i - 1]->classObject()) {
            return false;
        }
    }
    return true;
}

inline GAny GAnyFunction::dump() const
{
    GAny dumpObj = GAny::object();

    dumpObj["name"] = mName;
    GAny overloads = GAny::array();
    dumpObj["overloads"] = overloads;

    const GAnyFunction *overload = this;
    while (overload) {
        GAny ovi = GAny::object();
        overloads.pushBack(ovi);
        ovi["doc"] = overload->mDoc;
        if (overload->mDoCheckArgs) {
            ovi["return"] = overload->mArgTypes[0].as<GAnyClass>().getName();
            GAny args = GAny::array();
            ovi["args"] = args;
            for (size_t i = 1; i < overload->mArgTypes.size(); i++) {
                GAny arg = GAny::object();
                if (overload->mIsMethod && i == 1 &&
                    mName.substr(0, 6) != metaFunctionNames()[(size_t) MetaFunction::Init]) {
                    arg["key"] = "self";
                    arg["type"] = overload->mArgTypes[i].as<GAnyClass>().getName();
                } else {
                    std::stringstream argNameSS;
                    argNameSS << "arg" << i - 1;
                    arg["key"] = argNameSS.str();
                    arg["type"] = overload->mArgTypes[i].as<GAnyClass>().getName();
                }
                args.pushBack(arg);
            }
        }

        if (!overload->mNext.isFunction()) {
            break;
        }
        overload = &overload->mNext.as<GAnyFunction>();
    }

    std::stringstream docSS;
    docSS << *this;

    dumpObj["doc"] = docSS.str();

    return dumpObj;
}

template<typename Func, typename Return, typename... Args>
void GAnyFunction::initialize(Func &&f, Return (*)(Args...), const std::string &doc)
{
    this->mArgTypes = {GAnyClass::instance<Return>(), GAnyClass::instance<Args>()...};
    this->mDoc = doc;

    this->mFunc = [this, f](const GAny **args, int32_t argc) -> GAny {
        using indices = detail::make_index_sequence<sizeof...(Args)>;
        return call_impl(f, (Return(*)(Args...))
        nullptr, args, indices{});
    };
}

/// ================ CppType ================

inline CppType::CppType(std::type_index typeIndex)
        : mType(typeIndex)
{
    static std::map<std::type_index, std::pair<int32_t, AnyType>> lut = {
            {typeid(void),                    {0,  AnyType::undefined_t}},
            {typeid(nullptr),                 {1,  AnyType::null_t}},
            {typeid(bool),                    {2,  AnyType::boolean_t}},
            {typeid(char),                    {3,  AnyType::int8_t}},
            {typeid(int8_t),                  {4,  AnyType::int8_t}},
            {typeid(uint8_t),                 {5,  AnyType::int8_t}},
            {typeid(int16_t),                 {6,  AnyType::int16_t}},
            {typeid(uint16_t),                {7,  AnyType::int16_t}},
            {typeid(int32_t),                 {8,  AnyType::int32_t}},
            {typeid(uint32_t),                {9,  AnyType::int32_t}},
            {typeid(int64_t),                 {10, AnyType::int64_t}},
            {typeid(uint64_t),                {11, AnyType::int64_t}},
            {typeid(long),                    {12, AnyType::int64_t}},
            {typeid(float),                   {13, AnyType::float_t}},
            {typeid(double),                  {14, AnyType::double_t}},
            {typeid(std::string),             {15, AnyType::string_t}},
            {typeid(GAnyArray),               {16, AnyType::array_t}},
            {typeid(GAnyObject),              {17, AnyType::object_t}},
            {typeid(GAnyFunction),            {18, AnyType::function_t}},
            {typeid(GAnyClass),               {19, AnyType::class_t}},
            {typeid(GAnyClass::GAnyProperty), {20, AnyType::property_t}},
            {typeid(GAnyClass::GAnyEnum),     {21, AnyType::enum_t}},
            {typeid(GAnyException),           {22, AnyType::exception_t}},
            {typeid(GAnyCaller),              {23, AnyType::caller_t}},
            {typeid(GAny),                    {24, AnyType::user_obj_t}}
    };
    auto it = lut.find(typeIndex);
    if (it != lut.end()) {
        const auto &iti = it->second;
        mBasicTypeIndex = iti.first;
        mAnyType = iti.second;
    } else {
        mAnyType = AnyType::user_obj_t;
    }
}

/// ================ GAnyCaller ================

inline GAny GAnyCaller::call(const GAny **args, int32_t argc) const
{
    return mObj.classObject()._call(mObj, mMethodName, args, argc);
}

/// ================ GAnyClass ================

inline GAnyClass::GAnyClass(std::string nameSpace, std::string name, std::string doc, const CppType &cppType)
        : mNameSpace(std::move(nameSpace)), mName(std::move(name)), mDoc(std::move(doc)),
          mCppType(cppType), mType(cppType.anyType())
{
    updateHash();
}

inline GAnyClass &GAnyClass::setNameSpace(const std::string &ns)
{
    mNameSpace = ns;
    updateHash();
    return *this;
}

inline GAnyClass &GAnyClass::setName(const std::string &name)
{
    mName = name;
    updateHash();
    return *this;
}

inline GAnyClass &GAnyClass::setDoc(const std::string &doc)
{
    mDoc = doc;
    return *this;
}

inline const std::string &GAnyClass::getNameSpace() const
{
    return mNameSpace;
}

inline const std::string &GAnyClass::getName() const
{
    return mName;
}

inline const std::string &GAnyClass::getDoc() const
{
    return mDoc;
}

inline GAnyClass &GAnyClass::func(const std::string &name, const GAny &function, const std::string &doc, bool isMethod)
{
    if (name.empty()) {
        throw GAnyException("GAnyClass::def function name cannot be empty.");
        return *this;
    }
    if (!function.isFunction()) {
        throw GAnyException("GAnyClass::def function must be a function.");
        return *this;
    }
    GAny *dest = &mAttr[name];
    if (dest->isFunction()) {
        dest = dest->overload(function);
        if (dest == nullptr) {
            return *this;
        }
    } else {
        *dest = function;
    }
    dest->as<GAnyFunction>().mIsMethod = isMethod;
    dest->as<GAnyFunction>().mName = mName + "." + name;
    dest->as<GAnyFunction>().mDoc = doc;

    if (fInit.isUndefined() && name == metaFunctionNames()[(size_t) MetaFunction::Init]) {
        fInit = function;
        if (mCppType == CppTypeP<DynamicClassObject>()) {
            makeConstructor(*dest);
        }
    }
    if (fGetItem.isUndefined() && name == metaFunctionNames()[(size_t) MetaFunction::GetItem]) {
        fGetItem = function;
    }
    if (fSetItem.isUndefined() && name == metaFunctionNames()[(size_t) MetaFunction::SetItem]) {
        fSetItem = function;
    }
    return *this;
}

inline GAnyClass &GAnyClass::func(MetaFunction metaFunc, const GAny &function, const std::string &doc, bool isMethod)
{
    return func(metaFunctionNames()[(size_t) metaFunc], function, doc, isMethod);
}

inline GAnyClass &GAnyClass::staticFunc(const std::string &name, const GAny &function, const std::string &doc)
{
    return func(name, function, doc, false);
}

inline GAnyClass &GAnyClass::staticFunc(MetaFunction metaFunc, const GAny &function, const std::string &doc)
{
    return staticFunc(metaFunctionNames()[(size_t) metaFunc], function, doc);
}

inline GAnyClass &GAnyClass::property(const std::string &name, const GAny &fGet, const GAny &fSet,
                                      const std::string &doc)
{
    mAttr[name] = GAnyProperty(name, doc, fGet, fSet);
    return *this;
}

inline GAnyClass &GAnyClass::defEnum(const std::string &name, const GAny &enumObj, const std::string &doc)
{
    if (!enumObj.isObject()) {
        throw GAnyException("EnumObj must be an object");
        return *this;
    }
    mAttr[name] = GAnyEnum(name, doc, enumObj);
    // Expand enumeration into a class attribute table.
    auto enumMap = enumObj.castAs<std::unordered_map<std::string, GAny>>();
    for (const auto &item: enumMap) {
        auto it = mAttr.find(item.first);
        if (it == mAttr.end()) {
            mAttr[item.first] = item.second;
        }
    }
    return *this;
}

inline GAnyClass &GAnyClass::inherit(const GAny &parent)
{
    if (parent.isClass()) {
        this->mParents.push_back(parent);
    }
    return *this;
}

inline void GAnyClass::registerToEnv(std::shared_ptr<GAnyClass> clazz)
{
    if (pfnGanyRegisterToEnv) {
        pfnGanyRegisterToEnv(&clazz);
    }
}

inline GAny GAnyClass::_call(const GAny &inst, const std::string &function, const GAny **args, int32_t argc) const
{
    std::stringstream sst;

    GAny attr = getAttr(function);
    if (attr.isFunction()) {
        try {
            const auto &func = attr.as<GAnyFunction>();
            if (func.mIsMethod) {
                if (inst.isUndefined()) {
                    throw GAnyException("Method should be called with self.");
                }

                auto tArgc = argc + 1;
                const GAny **tArgs = (const GAny **) alloca(sizeof(GAny *) * tArgc);
                tArgs[0] = &inst;
                for (int32_t i = 0; i < argc; i++) {
                    tArgs[i + 1] = args[i];
                }

                return func._call(tArgs, tArgc);
            } else {
                return func._call(args, argc);
            }
        } catch (GAnyException &e) {
            sst << e.what() << "\n";
        }
    }

    for (const GAny &p: this->mParents) {
        try {
            if (p.isClass()) {
                return p.as<GAnyClass>()._call(inst, function, args, argc);
            }
            return p.getItem(0).as<GAnyClass>()._call(p.getItem(1)(inst), function, args, argc);
        }
        catch (GAnyException &e) {
            sst << e.what();
            continue;
        }
    }
    std::string ssts = sst.str();
    if (ssts.empty()) {
        throw GAnyException("Class " + mName + " failed to call method " + function + ".");
    } else {
        throw GAnyException(ssts);
    }
    return GAny::undefined();
}

inline void GAnyClass::makeConstructor(GAny fVar)
{
    auto &f = fVar.as<GAnyFunction>();
    auto func = f.mFunc;
    f.mFunc = [this, func](const GAny **args, int32_t argc) -> GAny {
        GAny self = GAny::object();
        self.as<GAnyObject>().clazz = this;
        std::vector<GAny> args1 = {self};

        int32_t tArgc = argc + 1;
        const GAny **tArgs = (const GAny **) alloca(sizeof(GAny *) * tArgc);

        tArgs[0] = &self;
        for (int32_t i = 0; i < argc; i++) {
            tArgs[i + 1] = args[i];
        }

        func(tArgs, tArgc);
        return self;
    };
    if (f.mDoCheckArgs && (f.mArgTypes.begin() + 1 != f.mArgTypes.end())) {
        f.mArgTypes.erase(f.mArgTypes.begin() + 1);
    }
}

inline GAny GAnyClass::castToBase(const GAnyClass &targetClass, const GAny &inst)
{
    for (GAny &base: this->mParents) {
        if (base.isClass()) {
            auto r = base.as<GAnyClass>().castToBase(targetClass, inst);
            if (!r.isUndefined()) {
                return r;
            }
        } else {
            if (base.getItem(0).as<GAnyClass>() == targetClass) {
                return base.getItem(1)(inst);
            } else {
                auto r = base.getItem(0).as<GAnyClass>().castToBase(targetClass, inst);
                if (!r.isUndefined()) {
                    return r;
                }
            }
        }
    }
    return GAny::undefined();
}

inline void GAnyClass::updateHash()
{
    mHash = hashOfByte(mNameSpace.data(), mNameSpace.size());
    mHash = hashMerge(mHash, hashOfByte(mName.data(), mName.size()));
}

inline std::shared_ptr<GAnyClass> GAnyClass::_instance(CppType cppType)
{
    if (pfnGanyClassInstance) {
        std::shared_ptr<GAnyClass> cls;
        pfnGanyClassInstance(&cppType, &cls);
        return cls;
    }
    return nullptr;
}

inline bool GAnyClass::containsMember(const std::string &name) const
{
    GAny attr = getAttr(name);
    if (attr.isProperty() || attr.isFunction() || attr.isEnum()) {
        return true;
    }

    for (const GAny &p: mParents) {
        try {
            if (p.isClass()) {
                return p.as<GAnyClass>().containsMember(name);
            } else {
                return p.getItem(0).as<GAnyClass>().containsMember(name);
            }
        }
        catch (GAnyException &) {
            continue;
        }
    }

    return false;
}

inline GAny GAnyClass::findMember(const std::string &name) const
{
    GAny attr = getAttr(name);
    if (!attr.isUndefined()) {
        return attr;
    }
    for (const GAny &p: mParents) {
        if (p.isClass()) {
            attr = p.as<GAnyClass>().findMember(name);
        } else {
            attr = p.getItem(0).as<GAnyClass>().findMember(name);
        }

        if (!attr.isUndefined()) {
            return attr;
        }
    }
    return attr;
}

inline const std::vector<GAny> &GAnyClass::getParents() const
{
    return mParents;
}

inline const std::unordered_map<std::string, GAny> &GAnyClass::getAttributes() const
{
    return mAttr;
}

inline GAny GAnyClass::dump() const
{
    GAny dumpObj = GAny::object();
    dumpObj["class"] = this->mName;
    dumpObj["nameSpace"] = this->mNameSpace;
    dumpObj["doc"] = this->mDoc;

    GAny parents = GAny::array();
    dumpObj["parents"] = parents;
    if (!this->mParents.empty()) {
        for (const GAny &p: this->mParents) {
            GAny c;
            if (p.isClass()) {
                c = p;
            } else {
                c = p.getItem(0);
            }
            if (c.isClass()) {
                parents.pushBack(c.as<GAnyClass>().mName);
            }
        }
    }

    if (!this->mAttr.empty()) {
        std::vector<std::pair<std::string, GAny>> mFunctions;
        std::vector<std::pair<std::string, GAny>> mProperties;
        std::vector<std::pair<std::string, GAny>> mEnums;

        for (std::pair<std::string, GAny> it: this->mAttr) {
            if (it.second.isFunction()) {
                mFunctions.push_back(it);
            } else if (it.second.isProperty()) {
                mProperties.push_back(it);
            } else if (it.second.isEnum()) {
                mEnums.push_back(it);
            }
        }

        std::sort(mFunctions.begin(), mFunctions.end(),
                  [](const std::pair<std::string, GAny> &a, const std::pair<std::string, GAny> &b) {
                      return GString(a.first).toLower() < GString(b.first).toLower();
                  });
        std::sort(mProperties.begin(), mProperties.end(),
                  [](const std::pair<std::string, GAny> &a, const std::pair<std::string, GAny> &b) {
                      return GString(a.first).toLower() < GString(b.first).toLower();
                  });
        std::sort(mEnums.begin(), mEnums.end(),
                  [](const std::pair<std::string, GAny> &a, const std::pair<std::string, GAny> &b) {
                      return GString(a.first).toLower() < GString(b.first).toLower();
                  });

        GAny methods = GAny::array();
        dumpObj["methods"] = methods;
        for (const auto &it: mFunctions) {
            methods.pushBack(it.second.as<GAnyFunction>().dump());
        }

        GAny propertiesObj = GAny::array();
        dumpObj["properties"] = propertiesObj;
        for (const auto &it: mProperties) {
            propertiesObj.pushBack(it.second.as<GAnyProperty>().dump());
        }

        GAny enumsObj = GAny::array();
        dumpObj["enums"] = enumsObj;
        for (const auto &it: mEnums) {
            enumsObj.pushBack(it.second.as<GAnyClass::GAnyEnum>().dump());
        }
    }
    return dumpObj;
}

inline GAny GAnyClass::getAttr(const std::string &name) const
{
    auto it = mAttr.find(name);
    if (it != mAttr.end()) {
        return it->second;
    }
    return GAny::undefined();
}

inline GAny GAnyClass::getItem(const GAny &inst, const GAny &i) const
{
    std::stringstream sst;

    if (i.isString()) {
        GAny attr = getAttr(i.toString());
        if (attr.isProperty()) {
            auto &fGet = attr.as<GAnyClass::GAnyProperty>().fGet;
            if (fGet.isFunction()) {
                try {
                    return fGet(inst);
                } catch (GAnyException &e) {
                    sst << e.what();
                }
            }
        } else if (attr.isEnum()) {
            try {
                return attr.as<GAnyClass::GAnyEnum>().enumObj;
            } catch (GAnyException &e) {
                sst << e.what();
            }
        } else if (attr.isFunction()) {
            if (attr.as<GAnyFunction>().mIsMethod) {
                return GAnyCaller(inst, i.toString());
            }
            return attr;
        }
    }

    if (fGetItem.isFunction()) {
        try {
            return fGetItem(inst, i);
        } catch (GAnyException &e) {
            sst << e.what();
        }
    }

    GAny r = GAny::undefined();
    for (const GAny &p: mParents) {
        try {
            if (p.isClass()) {
                r = p.as<GAnyClass>().getItem(inst, i);
            } else {
                r = p.getItem(0).as<GAnyClass>().getItem(p.getItem(1)(inst), i);
            }
        }
        catch (GAnyException &e) {
            sst << e.what();
            continue;
        }

        if (!r.isUndefined()) {
            return r;
        }
    }

    throw GAnyException("Class " + mName + " don't know how to do getItem " + i.toString() + ". \n" + sst.str());
    return GAny::undefined();
}

inline bool GAnyClass::setItem(const GAny &inst, const GAny &i, const GAny &v)
{
    std::stringstream sst;

    if (i.isString()) {
        GAny attr = getAttr(i.toString());
        if (attr.isProperty()) {
            auto &fSet = attr.as<GAnyClass::GAnyProperty>().fSet;
            if (fSet.isFunction()) {
                try {
                    fSet(inst, v);
                    return true;
                } catch (GAnyException &e) {
                    sst << e.what();
                }
            }
        }
    }

    if (fSetItem.isFunction()) {
        try {
            fSetItem(inst, i, v);
            return true;
        } catch (GAnyException &e) {
            sst << e.what();
        }
    }

    bool ok = false;
    for (const GAny &p: mParents) {
        try {
            if (p.isClass()) {
                ok = p.castAs<GAnyClass>().setItem(inst, i, v);
            } else {
                ok = p.getItem(0).castAs<GAnyClass>().setItem(p.getItem(1)(inst), i, v);
            }
        }
        catch (GAnyException &e) {
            sst << e.what();
            continue;
        }
        if (ok) {
            return ok;
        }
    }

    throw GAnyException("Class " + mName + " don't know how to do setItem " + i.toString() + ". \n" + sst.str());
    return false;
}

/// ================ Enum ================

#define REF_ENUM_OPERATORS(ENUM_TYPE) \
    .func(MetaFunction::EqualTo, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self == b; }) \
    .func(MetaFunction::LessThan, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self < b; }) \
    .func(MetaFunction::BitNot, [](ENUM_TYPE &self) { return ~self; }) \
    .func(MetaFunction::BitOr, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self | b; }) \
    .func(MetaFunction::BitAnd, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self & b; }) \
    .func(MetaFunction::BitXor, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self ^ b; }) \
    .func(MetaFunction::ToInt32, [](ENUM_TYPE &self) { return (int32_t) self; })

#define REF_ENUM(ENUM_TYPE, NAME_SPACE, DOC) \
    do { \
        GAny ENUM_TYPE##EnumMap = GAny::object(); \
        for (int32_t i = 0; i < Enum##ENUM_TYPE##Count; i++) { \
            ENUM_TYPE##EnumMap[Enum##ENUM_TYPE##Strs()[i]] = Enum##ENUM_TYPE##Keys()[i]; \
        } \
        Class<ENUM_TYPE>(NAME_SPACE, GX_STRINGIZE(ENUM_TYPE), DOC) \
            .defEnum("Enum", ENUM_TYPE##EnumMap)   \
            .func(MetaFunction::ToString, [](ENUM_TYPE &self) { \
                return std::string(EnumToString(self)); \
            }) \
            REF_ENUM_OPERATORS(ENUM_TYPE); \
    } while (0)

#define REF_ENUM_FLAGS(ENUM_TYPE, NAME_SPACE, DOC) REF_ENUM(ENUM_TYPE, NAME_SPACE, DOC)

GX_NS_END

#endif //GX_GANY_H
