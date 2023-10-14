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


TEST(TestGAny, Type)
{
    GAny a = 1;
    GAny b = 2LL;
    GAny bl = 2L;
    GAny c = 3.0f;
    GAny d = 4.0;
    GAny e = "5";
    GAny f = GString("string");
    GAny g = {1, 2, 3};
    GAny h = {{"a", 1},
              {"b", 2}};
    GAny i = &func;
    GAny j = [](int a) { return a + 1; };
    GAny k = true;
    GAny l = nullptr;
    GAny m = GAny::null();
    GAny n = GAny::undefined();
    GAny o = std::vector<int>();
    GAny p = std::map<std::string, int>();
    GAny q = std::unordered_map<std::string, int>();

    EXPECT_EQ(a.type(), AnyType::int32_t);
    EXPECT_EQ(b.type(), AnyType::int64_t);
    EXPECT_EQ(bl.type(), AnyType::int64_t);
    EXPECT_EQ(c.type(), AnyType::float_t);
    EXPECT_EQ(d.type(), AnyType::double_t);
    EXPECT_EQ(e.type(), AnyType::string_t);
    EXPECT_EQ(f.type(), AnyType::user_obj_t);
    EXPECT_EQ(g.type(), AnyType::array_t);
    EXPECT_EQ(h.type(), AnyType::object_t);
    EXPECT_EQ(i.type(), AnyType::function_t);
    EXPECT_EQ(j.type(), AnyType::function_t);
    EXPECT_EQ(k.type(), AnyType::boolean_t);
    EXPECT_EQ(l.type(), AnyType::null_t);
    EXPECT_EQ(m.type(), AnyType::null_t);
    EXPECT_EQ(n.type(), AnyType::undefined_t);
    EXPECT_EQ(o.type(), AnyType::array_t);
    EXPECT_EQ(p.type(), AnyType::object_t);
    EXPECT_EQ(q.type(), AnyType::object_t);
    EXPECT_EQ(GEnv["Gx.GString"].type(), AnyType::class_t);

    EXPECT_TRUE(l == m);
}