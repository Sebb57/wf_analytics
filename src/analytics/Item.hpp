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
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

namespace wf::analytics {

struct Item {
    std::string slug;
    std::string name;
    std::string thumb;
    std::vector<std::string> tags;

    [[nodiscard]] static Item fromJson(const json::Json& item)
    {
        const json::Json texts = item.at("i18n").at(constants::DEFAULT_LANGUAGE);

        return {
            item.at("slug").get<std::string>(),
            texts.at("name").get<std::string>(),
            texts.at("thumb").get<std::string>(),
            item.at("tags").get<std::vector<std::string>>(),
        };
    }

    [[nodiscard]] bool hasTag(std::string_view tag) const { return std::ranges::find(this->tags, tag) != this->tags.end(); }
};

}

#endif /* ITEM_HPP_ */
