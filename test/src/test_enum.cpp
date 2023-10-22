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

DEF_ENUM_5(TestEnum, 0,
           A,
           B,
           C,
           D,
           E)

DEF_ENUM_FLAGS_4(TestFlag, uint8_t,
                 A, 0x01,
                 B, 0x02,
                 C, 0x04,
                 AB, 0x03)

TEST(GAnyTest, Enum)
{
    EXPECT_EQ(EnumTestEnumCount, 5);

    // C++ Enum
    EXPECT_STRCASEEQ(EnumToString(TestEnum::A), "A");
    EXPECT_STRCASEEQ(EnumToString(TestEnum::B), "B");
    EXPECT_STRCASEEQ(EnumToString(TestEnum::C), "C");
    EXPECT_STRCASEEQ(EnumToString(TestEnum::D), "D");
    EXPECT_STRCASEEQ(EnumToString(TestEnum::E), "E");

    // Reflection Enum
    REF_ENUM(TestEnum, "E", "TestEnum.");
    EXPECT_TRUE(GEnv["E"].contains("TestEnum"));

    auto AnyTestEnum = GEnv["E.TestEnum"];

    EXPECT_EQ(AnyTestEnum.getItem("A"), TestEnum::A);
    EXPECT_EQ(AnyTestEnum.getItem("B"), TestEnum::B);
    EXPECT_EQ(AnyTestEnum.getItem("C"), TestEnum::C);
    EXPECT_EQ(AnyTestEnum.getItem("D"), TestEnum::D);
    EXPECT_EQ(AnyTestEnum.getItem("E"), TestEnum::E);

    // Reflection Flag Enum
    REF_ENUM_FLAGS(TestFlag, "E", "TestFlag.");

    auto AnyTestFlag = GEnv["E.TestFlag"];

    EXPECT_EQ(AnyTestFlag.getItem("A") | AnyTestFlag.getItem("B"), TestFlag::AB);
    EXPECT_STRCASEEQ((AnyTestFlag.getItem("A") | AnyTestFlag.getItem("B")).toString().c_str(),
                     EnumToString(TestFlag::AB));
}