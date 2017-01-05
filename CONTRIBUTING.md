# Contributing

## Coding standards

The code used in the Varuna compiler should always be standard-compliant (C++14) and not depend on platform or compiler-specific features.

The coding standard used in Varuna is based on the
[LLVM Coding Standards](http://llvm.org/docs/CodingStandards.html).
The exceptions to it are below. Also, the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) are used to some extent.

These rules are guidelines, not laws.
They are there only to provide consistency.

* Exceptions and RTTI are allowed (not disabled), but not encouraged
* `#include <iostream>` is allowed only, if `spdlog` fails
* Naming style:
    * **Types** (classes, structs, enums...) in PascalCase
    * **Variables**, **functions** and **namespaces** in camelCase
    * **Enumerators** in ALL_CAPS

### Coding style

In the root folder of the repository, there is a `.clang_format`-file,
that should be used for formatting the code. It can be used as follows:

```sh
# In repository root folder
$ clang-format -style=file -i [file]
```

* Indentation width is 4 spaces (LLVM: 2)
* Allman style braces:  
```cpp
void func()
{
    if(condition)
    {
        std::cout << "Foo";
    }
}
```
* Line break after template declarations:  
```cpp
template <typename T>
void func(const T& t)
{
    std::cout << t;
}
```
* Pointers and references are aligned to the left:  
```cpp
int* ptr;
const std::string& str;
```
* Never insert a space before parens:
```cpp
if(condition)
{
    call();
}
```
* All `#include`s are separated with only one break and are ordered followingly:
    * (.cpp files only) Interface declaration file in "quotes"
    * fe/*.h in "quotes"
    * core/*.h in "quotes"
    * util/*.h in "quotes"
    * LLVM and subproject (llvm, clang etc.) headers in <angle brackets>
    * Third-party library includes in <angle brackets>
    * Standard library and system headers in <angle brackets>
```cpp
#include "class.h"
#include "util/StringUtils.h"
#include <spdlog.h>
#include <utility>
```
