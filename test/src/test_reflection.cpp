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

class Course
{
public:
    explicit Course(std::string name)
            : mName(std::move(name))
    {}

public:
    void setScore(int32_t score)
    {
        mScore = score;
    }

    int32_t getScore() const
    {
        return mScore;
    }

    std::string getName() const
    {
        return mName;
    }

private:
    std::string mName;
    int32_t mScore;
};

class Student
{
public:
    using SayHelloFunc = std::function<void()>;

public:
    explicit Student(std::string name, int32_t age)
            : mName(std::move(name)), mAge(age)
    {}

public:
    std::string getName() const
    {
        return mName;
    }

    void setAge(int32_t age)
    {
        mAge = age;
    }

    int32_t getAge() const
    {
        return mAge;
    }

    void setSayHelloFunc(SayHelloFunc func)
    {
        mSayHelloFunc = std::move(func);
    }

    void sayHello()
    {
        if (mSayHelloFunc) {
            mSayHelloFunc();
        }
        std::cout << "Nothing todo." << std::endl;
    }

    void sayHello(const std::string &content)
    {
        std::cout << content << std::endl;
    }

    void addCourses(const Course &course)
    {
        mCourses.emplace(std::make_pair(course.getName(), course));
    }

    int32_t getTotalScore() const
    {
        int32_t score = 0;
        for (const auto &i: mCourses) {
            score += i.second.getScore();
        }
        return score;
    }

private:
    std::string mName;
    int32_t mAge;
    std::unordered_map<std::string, Course> mCourses;
    SayHelloFunc mSayHelloFunc;
};

TEST(TestGAny, Reflection)
{
    GAny ClassCourse = GAnyClass::Class < Course > ();
    ClassCourse.as<GAnyClass>()
            .setName("Course")
            .setDoc("Class Course.")
            .func(MetaFunction::Init, [](std::string name) {
                return std::make_unique<Course>(std::move(name));
            })
            .property("score", &Course::getScore, &Course::setScore)
            .func("setScore", &Course::setScore)
            .func("getScore", &Course::getScore)
            .property("name", &Course::getName)
            .func("getName", &Course::getName);

    GAny ClassStudent = GAnyClass::Class < Student > ();
    ClassStudent.as<GAnyClass>()
            .setName("Student")
            .setDoc("Class Student.")
            .func(MetaFunction::Init, [](std::string name, int32_t age) {
                return std::make_unique<Student>(std::move(name), age);
            })
            .property("name", &Student::getName)
            .func("getName", &Student::getName)
            .property("age", &Student::getAge, &Student::setAge)
            .func("setAge", &Student::setAge)
            .func("getAge", &Student::getAge)
            .func("setSayHelloFunc", [](Student &self, const GAny &func) {
                // Lambda cannot complete type inference, use GAny to pass the function directly.
                if (func.isFunction()) {
                    self.setSayHelloFunc([func]() {
                        func();
                    });
                }
            })
            .func("sayHello", [](Student &self, const std::string &content) {
                self.sayHello(content);
            })
            .func("sayHello", [](Student &self) {
                self.sayHello();
            })
            .func("addCourses", &Student::addCourses)
            .func("getTotalScore", &Student::getTotalScore);

    GAny stud1 = ClassStudent("Bob", 15);
    EXPECT_TRUE(stud1.isUserObject());
}