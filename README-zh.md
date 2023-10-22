# GxAny：C++17 的动态类型系统
[English](README.md)

GxAny 是一个强大、灵活且高效的库，为 C++ 引入了动态类型系统。它与原生 C++ 类型的无缝集成、动态类型反射，以及一系列实用函数，使 C++ 中的动态类型变得简单。

## 特点：
- **动态类型**：使用 GAny 轻松地在 C++ 静态类型和动态类型之间切换。
- **类型安全**：在保持动态性的同时，GAny 确保类型安全并提供直观的类型转换。
- **动态反射**：在运行时动态地反射类和函数，甚至可以动态扩展类型功能。
- **动态类型创建**：运行时动态创建非语言绑定的类型，
- **非侵入式**：GAny实现动态反射不需要在目标类型中增加宏定义或特殊属性。
- **插件系统**：提供了纯头文件模块：gany-interface，旨在方便用户编写插件，然后被主程序加载并调用。
- **跨语言绑定**：方便绑定到各种语言，实现目标语言加载和调用C/C++编写的功能，同时也能实现在C++中调用其他语言编写的功能。

## 快速开始：
```cpp
#include <gx/gany_core.h>
#include <gx/gany.h>

using namespace gx;

int main(int argc, char *args[])
{
    initGAnyCore();

    GAny a = 10;
    GAny b = "你好，世界！";
    GAny c = [](int32_t x) { return x + 1; };

    std::cout << a.as<int32_t>() << std::endl;
    std::cout << b.as<std::string>() << std::endl;
    std::cout << c(5).as<int32_t>() << std::endl;

    return 0;
}
```

## 下载与使用
```shell
git clone git@github.com:giarld/GxAny.git
```
然后复制到您的项目,项目的 `CMakeList.txt` 中增加:
```cmake
add_subdirectory(gany-interface)
# 开发主模块时需要增加gany-core, 开发插件时仅增加gany-interface
add_subdirectory(gany-core)

# 当应用于主模块时
target_link_libraries(TARGET_NAME PRIVATE gany-core)

# 当应用于插件开发时
target_link_libraries(TARGET_NAME PRIVATE gany-interface)
```

## 许可：
`GxAny` 根据 [MIT许可证](LICENSE.txt) 授权。
