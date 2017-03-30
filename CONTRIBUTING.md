# Contributing

All contributions are welcome!

## Pull requests

It's highly recommended to open a issue for a discussion before making a pull request.

All pull requests should be made against the `develop` branch. Make sure the code is properly formatted and doesn't violate the coding standards and style. If you're adding a new feature, consider adding a test for it.

Compiler and language documentation and tests are unfortunately sincerely lacking right now.

## Coding standards

The code used in the Varuna compiler should always be standard-compliant (C++14) and it should avoid depending on platform-specific features.

The coding standard used in Varuna is based on the
[LLVM Coding Standards](http://llvm.org/docs/CodingStandards.html).
The exceptions to it are below. Also, the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines) are used to some extent.

These rules are guidelines, not laws.
They are there only to provide consistency.

Naming style:
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
    * Other Varuna headers in "quotes"
    * Third-party library includes in <angle brackets>
    * Standard library and system headers in <angle brackets>
```cpp
#include "class.h"
#include "util/StringUtils.h"
#include <spdlog.h>
#include <vector>
```
