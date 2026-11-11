/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Item.hpp
*/

#ifndef ITEM_HPP_
    #define ITEM_HPP_

#include "constants.hpp"
#include "json/Json.hpp"
#include <string>

namespace wf::analytics {

struct Item {
    std::string slug;
    std::string name;
    std::string thumb;

    [[nodiscard]] static Item fromJson(const json::Json& item)
    {
        const json::Json texts = item.at("i18n").at(constants::DEFAULT_LANGUAGE);

        return {
            item.at("slug").get<std::string>(),
            texts.at("name").get<std::string>(),
            texts.at("thumb").get<std::string>(),
        };
    }
};

}

#endif /* ITEM_HPP_ */
