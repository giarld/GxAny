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

class BaseClass
{
public:
    int32_t baseFunction() const
    {
        return 114;
    }
};

class MyClass : public BaseClass
{
public:
    MyClass(int32_t val)
            : value(val)
    {}

    void setValue(int32_t val)
    {
        value = val;
    }

    int32_t getValue() const
    {
        return value;
    }

private:
    int32_t value;
};


TEST(GAnyReflectionTest, ReflectionWithGAnyClass)
{
    // Register BaseClass type
    Class<BaseClass>("MyNamespace", "BaseClass", "Base class for testing inheritance")
            .func("baseFunction", &BaseClass::baseFunction, "Base class function");

    Class<MyClass>("MyNamespace", "MyClass", "Description for MyClass")
            .inherit<BaseClass>()
            .construct<int32_t>()
            .func("setValue", [](MyClass &self, int32_t newVal) {
                self.setValue(newVal);
            }, "Set value for MyClass")
            .func("getValue", [](MyClass &self) -> int32_t {
                return self.getValue();
            }, "Get value from MyClass")
            .property("value", &MyClass::getValue, &MyClass::setValue)
            .defEnum("MyEnum", {
                    {"ENUM_A", 1},
                    {"ENUM_B", 2},
                    {"ENUM_C", 3}
            }, "An example enum");

    // Create MyClass object using reflection
    auto MyType = GEnv["MyNamespace.MyClass"];
    auto myObj = MyType(10);  // Initialize to 10 using constructor

    // Get and set the value of MyClass using reflection
    EXPECT_EQ(myObj.call("getValue").as<int32_t>(), 10);  // Initial value should be 10
    EXPECT_NO_THROW(myObj.call("setValue", 20));
    EXPECT_EQ(myObj.call("getValue").as<int32_t>(), 20);  // Value after setting should be 20

    // Test properties
    EXPECT_EQ(myObj.getItem("value").as<int32_t>(), 20);  // Value obtained through property should be 20

    // Test inheritance
    EXPECT_EQ(myObj.call("baseFunction").toInt32(), 114);  // Should print "Base function called"

    // Test enums
    EXPECT_EQ(MyType.getItem("MyEnum.ENUM_A").toInt32(), 1);
    EXPECT_EQ(MyType.getItem("MyEnum.ENUM_B").toInt32(), 2);
    EXPECT_EQ(MyType.getItem("MyEnum.ENUM_C").toInt32(), 3);

    MyClass &myObjCpp = myObj.as<MyClass>();
    // Convert to C++object and call
    EXPECT_EQ(myObjCpp.getValue(), 20);

    const BaseClass &myObjBase = myObj.castAs<BaseClass>();
    // Convert to Base Class Object
    EXPECT_EQ(myObjBase.baseFunction(), 114);
}

TEST(GAnyReflectionTest, DirectTypeCreationWithGAnyClass)
{
    // Directly create a new type named 'DynamicType' using GAnyClass
    auto DynamicType = GAnyClass::Class("", "DynamicType", "Directly created type for testing purposes");
    DynamicType->func(MetaFunction::Init, [](GAny &self, int32_t x, int32_t y) {
                self.setItem("x", x);
                self.setItem("y", y);
            }, "Constructor for DynamicType")
            .func("setX", [](GAny &self, int32_t x) {
                self.setItem("x", x);
            }, "Set x for DynamicType")
            .func("getX", [](GAny &self) -> int32_t {
                return self.getItem("x").as<int32_t>();
            }, "Get x from DynamicType")
            .func("print", [](GAny &self) {
                printf("print: %s\n", self.toString().c_str());
            })
            .func(MetaFunction::ToString, [](GAny &self) -> std::string {
                std::stringstream ss;
                ss << "DynamicType x = " << self.getItem("x").as<int32_t>() << ", y = " << self.getItem("y").as<int32_t>();
                return ss.str();
            }, "Convert DynamicType to string")
            .defEnum(
                    "DynamicEnum",
                    {
                            {"VAL_1", 1},
                            {"VAL_2", 2},
                            {"VAL_3", 3}
                    }, "An example enum for DynamicType");

    // Register the dynamically created type to GEnv
    GAnyClass::registerToEnv(DynamicType);

    // Create an instance of the dynamically created type using the constructor
    auto tDynamicType = GEnv["DynamicType"];
    auto dynObj = tDynamicType(10, 20);  // Initialize x with 10 and y with 20

    // Use the instance to call functions and access properties
    EXPECT_EQ(dynObj.call("getX").as<int32_t>(), 10);  // Initial x should be 10
    EXPECT_NO_THROW(dynObj.call("setX", 30));
    EXPECT_EQ(dynObj.call("getX").as<int32_t>(), 30);  // After setting, x should be 30

    // Test enum values
    EXPECT_EQ(tDynamicType.getItem("DynamicEnum.VAL_1").toInt32(), 1);
    EXPECT_EQ(tDynamicType.getItem("DynamicEnum.VAL_2").toInt32(), 2);
    EXPECT_EQ(tDynamicType.getItem("DynamicEnum.VAL_3").toInt32(), 3);
}