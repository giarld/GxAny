# GxAny: A Dynamic Typing System for C++17
[中文](README-zh.md)

GxAny is a powerful, flexible, and efficient library that introduces a dynamic typing system to C++. It seamlessly integrates with native C++ types, offers dynamic type reflection, and a series of utility functions, making dynamic typing in C++ straightforward.

## Features
- **Dynamic Typing**: Switch between C++ static types and dynamic types with ease using GAny.
- **Type Safety**: While maintaining dynamism, GAny ensures type safety and provides intuitive type conversions.
- **Dynamic Reflection**: Dynamically reflect classes and functions at runtime, even dynamically extending type functionalities.
- **Dynamic Creation**: Create non-language bound types at runtime.
- **Non-intrusive**: GAny's implementation of dynamic reflection doesn't require adding macros or special attributes in the target type.
- **Plugin System**: Provides a header-only module, gany-interface, aimed at facilitating users to write plugins that can then be loaded and invoked by the main program.
- **Cross-Language Binding**: Conveniently binds to various languages, allowing target languages to load and invoke functionalities written in C/C++, and also facilitates invoking functionalities of other languages in C++.

## Quick Start
```cpp
#include <gx/gany_core.h>
#include <gx/gany.h>

using namespace gx;

int main(int argc, char *args[])
{
    initGAnyCore();

    GAny a = 10;
    GAny b = "Hello, World!";
    GAny c = [](int32_t x) { return x + 1; };

    std::cout << a.as<int32_t>() << std::endl;
    std::cout << b.as<std::string>() << std::endl;
    std::cout << c(5).as<int32_t>() << std::endl;

    return 0;
}
```

## Just like using JSON
```cpp
#include <gx/gany_core.h>
#include <gx/gany.h>

using namespace gx;

int main(int argc, char *args[])
{
    initGAnyCore();

    GAny obj = GAny::object();
    obj["name"] = "Alice";
    obj["age"] = 28;
    obj["isStudent"] = false;
    GAny arr = GAny::array();
    arr.pushBack(1);
    arr.pushBack(2);
    arr.pushBack(3);
    obj["numbers"] = arr;

    std::cout << obj.toJsonString(2) << std::endl;

    return 0;
}
```

## Dynamic reflection
```cpp
#include <gx/gany.h>
#include <gx/gany_core.h>

using namespace gx;

class IPair
{
public:
    explicit IPair(int32_t a, int32_t b)
            : mA(a), mB(b)
    {}

public:
    int32_t getA() const
    {
        return mA;
    }

    void setA(int32_t a)
    {
        mA = a;
    }

    int32_t getB() const
    {
        return mB;
    }

    void setB(int32_t b)
    {
        mB = b;
    }

    IPair operator+(const IPair &b)
    {
        IPair t = *this;
        t.mA += b.mA;
        t.mB += b.mB;
        return t;
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "[" << mA << "," << mB << "]";
        return ss.str();
    }

private:
    int32_t mA;
    int32_t mB;
};

int main(int argc, char *argv[])
{
    initGAnyCore();

    Class<IPair>("G", "IPair", "Class IPair.")
            .construct<int32_t, int32_t>()
            .func("getA", &IPair::getA)
            .func("setA", &IPair::setA)
            .func("getB", &IPair::getB)
            .func("setB", &IPair::setB)
            .func(MetaFunction::Addition, &IPair::operator+)
            .func(MetaFunction::ToString, &IPair::toString)
            .func(MetaFunction::ToObject, [](IPair &self) {
                GAny array = GAny::array();
                array.pushBack(self.getA());
                array.pushBack(self.getB());
                return array;
            })
            .property("a", &IPair::getA, &IPair::setA)
            .property("b", &IPair::getB, &IPair::setB);

    auto tIPair = GEnv["G.IPair"];
    auto pA = tIPair(1, 2);
    auto pB = tIPair(9, 8);

    pA.set("a", 3);
    std::cout << "pA.a = " << pA.get<int32_t>("a", 0) << std::endl;
    std::cout << "pA.b = " << pA.getItem("b") << std::endl;

    pB.call("setB", 5);
    std::cout << "pB.b = " << pB.call("getB").as<int32_t>() << std::endl;

    auto pC = pA + pB;  // Call Addition
    std::cout << "pC = " << pC.toString() << std::endl; // Call ToString

    return EXIT_SUCCESS;
}
```

## Example of plugin development
[GxAnyPluginExample](https://github.com/giarld/GxAnyPluginExample)

## Download & Usage
```shell
git clone git@github.com:giarld/GxAny.git
```
Then copy to your project. Add the following to your project's CMakeList.txt:
```cmake
add_subdirectory(gany-interface)
# Add gany-core when developing the main module, and only add gany-interface when developing plugins.
add_subdirectory(gany-core)

# When applied to the main module:
target_link_libraries(TARGET_NAME PRIVATE gany-core)

# When applied for plugin development:
target_link_libraries(TARGET_NAME PRIVATE gany-interface)
```

## Third party libraries used
- [rapidjson](https://github.com/Tencent/rapidjson)
- [gtest](https://github.com/google/googletest)

## License
`GxAny` is licensed under the [MIT License](LICENSE.txt).
