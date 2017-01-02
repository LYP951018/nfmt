#include "Fmt.hpp"

using namespace std::string_literals;

FormatError::FormatError(std::size_t pos)
    : invalid_argument{ "Error format at "s + std::to_string(pos) + "." }
{
}

FormatError::FormatError(std::string_view errorMessage)
    : invalid_argument{ std::string{ errorMessage } }
{
}
