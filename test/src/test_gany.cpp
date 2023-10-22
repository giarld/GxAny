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

#include <gtest/gtest.h>

#include <gx/gany_core.h>
#include <gx/gany.h>


using namespace gx;

int func(int a, int b)
{
    return a + b;
}

TEST(GAnyTest, BasicConstruction)
{
    GAny defaultAny;
    EXPECT_EQ(defaultAny.type(), AnyType::undefined_t);
    EXPECT_TRUE(defaultAny.isUndefined());

    GAny nullAny = nullptr;
    EXPECT_EQ(nullAny.type(), AnyType::null_t);
    EXPECT_TRUE(nullAny.isNull());

    GAny int32Any = 123;
    EXPECT_EQ(int32Any.type(), AnyType::int32_t);
    EXPECT_TRUE(int32Any.isInt32());
    EXPECT_EQ(int32Any.as<int32_t>(), 123);

    GAny int64Any = (int64_t) 123;
    EXPECT_EQ(int64Any.type(), AnyType::int64_t);
    EXPECT_TRUE(int64Any.isInt64());
    EXPECT_EQ(int64Any.as<int64_t>(), (int64_t) 123);

    GAny floatAny = 3.14f;
    EXPECT_EQ(floatAny.type(), AnyType::float_t);
    EXPECT_TRUE(floatAny.isFloat());
    EXPECT_FLOAT_EQ(floatAny.as<float>(), 3.14f);

    GAny doubleAny = 3.14;
    EXPECT_EQ(doubleAny.type(), AnyType::double_t);
    EXPECT_TRUE(doubleAny.isDouble());
    EXPECT_FLOAT_EQ(doubleAny.as<double>(), 3.14);

    GAny stringAny = "Hello";
    EXPECT_EQ(stringAny.type(), AnyType::string_t);
    EXPECT_TRUE(stringAny.isString());
    EXPECT_EQ(stringAny.as<std::string>(), "Hello");

    GAny boolAny = true;
    EXPECT_EQ(boolAny.type(), AnyType::boolean_t);
    EXPECT_TRUE(boolAny.isBoolean());
    EXPECT_EQ(boolAny.as<bool>(), true);

    GAny userObjAny = GString("string");
    EXPECT_EQ(userObjAny.type(), AnyType::user_obj_t);
    EXPECT_TRUE(userObjAny.isUserObject());

    GAny arrayAny = {1, 2, 3};
    EXPECT_EQ(arrayAny.type(), AnyType::array_t);
    EXPECT_TRUE(arrayAny.isArray());

    GAny objectAny = {{"a", 1},
                      {"b", 2}};
    EXPECT_EQ(objectAny.type(), AnyType::object_t);
    EXPECT_TRUE(objectAny.isObject());

    GAny func1Any = &func;
    EXPECT_EQ(func1Any.type(), AnyType::function_t);
    EXPECT_TRUE(func1Any.isFunction());

    GAny func2Any = [](int32_t a) { return a + 1; };
    EXPECT_EQ(func2Any.type(), AnyType::function_t);
    EXPECT_TRUE(func2Any.isFunction());
}

TEST(GAnyTest, ArithmeticOperations)
{
    GAny a(10);
    GAny b(5);

    EXPECT_EQ((a + b).as<int32_t>(), 15);
    EXPECT_EQ((a - b).as<int32_t>(), 5);
    EXPECT_EQ((a * b).as<int32_t>(), 50);
    EXPECT_EQ((a / b).as<int32_t>(), 2);
}

TEST(GAnyTest, ObjectTypeAndOperations)
{
    GAny obj = GAny::object();
    obj["name"] = "John";
    obj["age"] = 30;

    EXPECT_EQ(obj["name"].as<std::string>(), "John");
    EXPECT_EQ(obj["age"].as<int32_t>(), 30);
    EXPECT_TRUE(obj.contains("name"));
    obj.erase("name");
    EXPECT_FALSE(obj.contains("name"));
}

TEST(GAnyTest, ArrayTypeAndOperations)
{
    GAny arr = GAny::array(std::vector<GAny>{1, 2, 3});
    EXPECT_EQ(arr[0].as<int32_t>(), 1);
    EXPECT_EQ(arr[1].as<int32_t>(), 2);
    EXPECT_EQ(arr[2].as<int32_t>(), 3);

    arr.pushBack(4);
    EXPECT_EQ(arr[3].as<int32_t>(), 4);
    EXPECT_EQ(arr.size(), 4);
}

TEST(GAnyTest, FunctionTypeAndOverloads)
{
    GAny func = [](int32_t a) { return a + 1; };
    func.overload([](int32_t a, int32_t b) { return a + b; });
    func.overload([](const std::string &s) { return s + " World!"; });

    EXPECT_EQ(func(2).as<int32_t>(), 3);
    EXPECT_EQ(func(2, 3).as<int32_t>(), 5);
    EXPECT_EQ(func("Hello").as<std::string>(), "Hello World!");
}

TEST(GAnyTest, Cloning)
{
    GAny original(42);
    GAny clone = original.clone();
    EXPECT_EQ(clone.as<int32_t>(), 42);
}

TEST(GAnyTest, UserObject)
{
    GAny userObj = GString("Hello");
    GAny subStr = userObj.call("left", 3);

    EXPECT_TRUE(userObj.isUserObject());
    EXPECT_TRUE(subStr.isUserObject());
    EXPECT_EQ(subStr.toString(), "Hel");
}

TEST(GAnyTest, BasicToJson)
{
    GAny obj = GAny::object();
    obj["name"] = "Alice";
    obj["age"] = 28;
    obj["isStudent"] = false;
    GAny arr = GAny::array();
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    obj["numbers"] = arr;

    std::string jsonStr = obj.toJsonString();
    EXPECT_EQ(jsonStr.size(), 61);
}

TEST(GAnyTest, BasicFromJson)
{
    std::string jsonStr = R"({"name":"Bob","age":22,"isStudent":true,"courses":["Math","Physics"]})";
    GAny obj = GAny::parseJson(jsonStr);

    EXPECT_TRUE(obj.isObject());
    EXPECT_EQ(obj["name"].as<std::string>(), "Bob");
    EXPECT_EQ(obj["age"].as<int32_t>(), 22);
    EXPECT_TRUE(obj["isStudent"].as<bool>());
    EXPECT_TRUE(obj["courses"].isArray());
    EXPECT_EQ(obj["courses"][0].as<std::string>(), "Math");
    EXPECT_EQ(obj["courses"][1].as<std::string>(), "Physics");
}

TEST(GAnyTest, NestedJson)
{
    std::string jsonStr = R"({"person":{"name":"Charlie","age":30,"address":{"city":"New York","zipcode":"10001"}}})";
    GAny obj = GAny::parseJson(jsonStr);

    EXPECT_TRUE(obj.isObject());
    EXPECT_TRUE(obj["person"].isObject());
    EXPECT_EQ(obj["person"]["name"].as<std::string>(), "Charlie");
    EXPECT_EQ(obj["person"]["age"].as<int32_t>(), 30);
    EXPECT_TRUE(obj["person"]["address"].isObject());
    EXPECT_EQ(obj["person"]["address"]["city"].as<std::string>(), "New York");
    EXPECT_EQ(obj["person"]["address"]["zipcode"].as<std::string>(), "10001");
}


TEST(GAnyCastTest, BasicTypes)
{
    // int
    GAny intVal(123);
    EXPECT_EQ(intVal.castAs<int32_t>(), 123);

    // double
    GAny doubleVal(12.34);
    EXPECT_DOUBLE_EQ(doubleVal.castAs<double>(), 12.34);

    // string
    GAny strVal("hello");
    EXPECT_EQ(strVal.castAs<std::string>(), "hello");

    // bool
    GAny boolVal(true);
    EXPECT_EQ(boolVal.castAs<bool>(), true);
}

TEST(GAnyCastTest, ComplexTypes)
{
    // Array
    GAny arr = GAny::array();
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    std::vector<GAny> vec = arr.castAs<std::vector<GAny>>();
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0].as<int32_t>(), 1);
    EXPECT_EQ(vec[1].as<int32_t>(), 2);
    EXPECT_EQ(vec[2].as<int32_t>(), 3);

    // Object
    GAny obj = GAny::object();
    obj["name"] = "Alice";
    obj["age"] = 28;
    std::unordered_map<std::string, GAny> map = obj.castAs<std::unordered_map<std::string, GAny>>();
    EXPECT_EQ(map["name"].as<std::string>(), "Alice");
    EXPECT_EQ(map["age"].as<int32_t>(), 28);
}

TEST(GAnyCastTest, TypeConversions)
{
    GAny strVal("123");
    EXPECT_EQ(strVal.castAs<int32_t>(), 123); // String to int
    EXPECT_DOUBLE_EQ(strVal.castAs<double>(), 123.0); // String to double

    GAny numVal(123);
    EXPECT_EQ(numVal.castAs<std::string>(), "123"); // Int to string

    GAny doubleVal(123.45);
    EXPECT_EQ(doubleVal.castAs<std::string>(), "123.45"); // Double to string
}

TEST(GAnyCastTest, NumericConversions)
{
    // Int to double
    GAny intVal(123);
    EXPECT_DOUBLE_EQ(intVal.castAs<double>(), 123.0);

    // Double to int
    GAny doubleVal(123.45);
    EXPECT_EQ(doubleVal.castAs<int32_t>(), 123);

    // Float to int
    GAny floatVal(456.78f);
    EXPECT_EQ(floatVal.castAs<int32_t>(), 456);

    // Int to float
    GAny intValue(789);
    EXPECT_FLOAT_EQ(intValue.castAs<float>(), 789.0f);

    // Double to float and vice versa
    doubleVal = 321.65;
    EXPECT_FLOAT_EQ(doubleVal.castAs<float>(), 321.65f);
    floatVal = 654.32f;
    EXPECT_NEAR(floatVal.castAs<double>(), 654.32, 0.001);
}
