# nfmt
A naive C++ format library implementation with C++ 17.

#Usage

```cpp
#include "Fmt.hpp"
#include <string>

int main()
{
    using namespace std::string_literals;
    const auto str = Format("I'm {} years old and grade {}.\n {} {{}}", 2, 3, "This is a brace "s);
}
```
