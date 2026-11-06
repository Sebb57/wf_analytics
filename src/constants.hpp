/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** constants.hpp
*/

#ifndef CONSTANTS_HPP_
    #define CONSTANTS_HPP_

#include <string_view>
namespace wf::constants {

inline constexpr int SUCCESS = 0;
inline constexpr int FAILURE = 84;

inline constexpr std::string_view API_URL = "https://api.warframe.market/v2/";
inline constexpr std::string_view ITEMS = "items";

}

#endif /* CONSTANTS_HPP_ */

