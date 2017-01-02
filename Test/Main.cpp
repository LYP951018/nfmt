#include "Fmt.hpp"

int main()
{
    const auto str = Format("I'm {} years old and grade {}.\n This is a brace: {{}}", 2, 3);
}