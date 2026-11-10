/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** constants.hpp
*/

#ifndef CONSTANTS_HPP_
    #define CONSTANTS_HPP_

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace wf::constants {

template <const std::string_view&... Parts>
struct Concat {
    static constexpr std::size_t size = (Parts.size() + ...);
    static constexpr std::array<char, size + 1> buffer = [] {
        std::array<char, size + 1> result{};
        auto out = result.begin();

        ((out = std::copy(Parts.begin(), Parts.end(), out)), ...);
        return result;
    }();
};

template <const std::string_view&... Parts>
inline constexpr std::string_view concat{Concat<Parts...>::buffer.data(), Concat<Parts...>::size};

inline constexpr int SUCCESS = 0;
inline constexpr int FAILURE = 1;

// client
inline constexpr std::string_view USER_AGENT = "MyWarframeApp/1.0 (C++ client)";
inline constexpr std::string_view DEFAULT_LANGUAGE = "en";
inline constexpr double REQUESTS_PER_SECOND = 3.0;

// versions
inline constexpr std::string_view PRODUCTION = "v2/";

// urls
inline constexpr std::string_view BASE_URL = "https://api.warframe.market/";
inline constexpr std::string_view PRODUCTION_URL = concat<BASE_URL, PRODUCTION>;

// routes of PRODUCTION_URL
inline constexpr std::string_view VERSIONS = "versions";
inline constexpr std::string_view ITEMS = "items";
inline constexpr std::string_view ITEM = "item/";                   // item/<slug>
inline constexpr std::string_view ITEM_SET = "/set";                // item/<slug>/set
inline constexpr std::string_view ORDERS_RECENT = "orders/recent";
inline constexpr std::string_view ORDERS_ITEM = "orders/item/";     // orders/item/<slug>
inline constexpr std::string_view ORDERS_TOP = "/top";              // orders/item/<slug>/top
inline constexpr std::string_view RIVEN_WEAPONS = "riven/weapons";
inline constexpr std::string_view RIVEN_ATTRIBUTES = "riven/attributes";

}

#endif /* CONSTANTS_HPP_ */
