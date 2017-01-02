# nfmt
A naive C++ format library implementation with C++ 17.

#Usage

```cpp
#include "Fmt.hpp"

int main()
{
    const auto str = Format("I'm {} years old and grade {}.\n This is a brace: {{}}", 2, 3);
    //"I'm 2 years old and grade 3.\n This is a brace: {}"
}
```
