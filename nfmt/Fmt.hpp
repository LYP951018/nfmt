#pragma once

#include <string>
#include <variant>
#include <array>            //std::array
#include <tuple>            //std::tuple std::tuple_cat
#include <functional>       //std::reference_wrapper
#include <stdexcept>
#include <type_traits>
#include <string_view>      //std::string_view

class FormatError : public std::invalid_argument
{
public:
    FormatError(std::size_t pos);
    FormatError(std::string_view errorMessage);
};

namespace detail
{
    using std::to_string;

    struct AppendVisitor
    {
        std::string& str_;

        AppendVisitor(std::string& s) noexcept
            : str_{ s }
        {}

        template<typename T, typename = std::void_t<decltype(to_string(std::declval<T>()))>>
        void operator()(std::reference_wrapper<T> ref) const
        {
            using std::to_string;
            str_.append(to_string(ref.get()));
        }

        void operator()(std::reference_wrapper<const std::string> ref) const
        {
            str_.append(ref.get());
        }

        void operator()(std::reference_wrapper<const std::string_view> ref) const
        {
            str_.append(ref.get());
        }
    };

    template<typename... Args>
    void Append(std::string& str, const std::variant<Args...>& v)
    {
        std::visit(AppendVisitor{ str }, v);
    }

    template<typename T, typename... Args>
    using is_one_of = std::disjunction<std::is_same<T, Args>...>;

    template<typename... Args>
    struct Unique;

    template<typename T, typename... Args>
    struct Unique<T, Args...>
    {
        using type = decltype(std::tuple_cat(std::declval<std::conditional_t<is_one_of<T, Args...>::value,
            std::tuple<>,
            std::tuple<T>>>(), std::declval<typename Unique<Args...>::type>()));
    };

    template<typename T>
    struct Unique<T>
    {
        using type = std::tuple<T>;
    };

    template<typename... Args>
    using UniqueType = typename Unique<Args...>::type;

    template<typename... Args>
    std::variant<Args...> FromTuple(std::tuple<Args...>);

    template<typename... Args>
    using UniqueVariant = decltype(FromTuple(std::declval<UniqueType<Args...>>()));
}


template<typename... Args>
std::string Format(std::string_view format, const Args&... args)
{
    std::string result;
    auto current = format.begin();
    std::array<detail::UniqueVariant<std::reference_wrapper<std::add_const_t<Args>>...>, sizeof...(Args)> argsList{ std::ref(args)... };
    std::size_t currentArg = {};

    const auto ThrowErrorAt = [format](typename std::string_view::iterator pos)
    {
        throw FormatError{ static_cast<std::size_t>(pos - format.begin()) };
    };

    using namespace std::string_literals;
    while (true)
    {
        if (current == format.end()) break;
        const auto c = *current;
        switch (c)
        {
        case '{':
            if (current + 1 != format.end())
            {
                const auto nextChar = current[1];
                if (nextChar == '{')
                {
                    result.push_back('{');
                    ++current;
                }
                else if (nextChar == '}')
                {
                    if (currentArg >= argsList.size())
                    {
                        throw FormatError{ "Argument out of bound at pos "s + std::to_string(current - format.begin()) };
                    }
                    detail::Append(result, argsList[currentArg]);
                    ++current;
                    ++currentArg;
                }
                else
                {
                    ThrowErrorAt(current);
                }
            }
            break;
        case '}':
            if (current + 1 != format.end())
            {
                const auto nextChar = current[1];
                if (nextChar == '}')
                {
                    result.push_back('}');
                    ++current;
                }
                else
                {
                    ThrowErrorAt(current);
                }
            }
            break;
        default:
            result.push_back(c);
            break;
        }
        ++current;
    }
    return result;
}