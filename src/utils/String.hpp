/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** String.hpp
*/

#ifndef STRING_HPP_
    #define STRING_HPP_

#include <algorithm>
#include <cctype>
#include <charconv>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

namespace wf::utils {

[[nodiscard]] inline std::string lower(std::string text)
{
    std::ranges::transform(text, text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

// text without any occurrence of part
[[nodiscard]] inline std::string erase(std::string text, std::string_view part)
{
    for (std::size_t at = text.find(part); at != std::string::npos; at = text.find(part, at))
        text.erase(at, part.size());
    return text;
}

// number written in text, nothing if it is not one
[[nodiscard]] inline std::optional<double> toDouble(std::string_view text)
{
    double value = 0.0;

    if (std::from_chars(text.data(), text.data() + text.size(), value).ec != std::errc{})
        return std::nullopt;
    return value;
}

}

#endif /* STRING_HPP_ */
