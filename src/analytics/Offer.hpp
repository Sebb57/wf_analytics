/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Offer.hpp
*/

#ifndef OFFER_HPP_
    #define OFFER_HPP_

#include "analytics/Item.hpp"
#include "json/Json.hpp"
#include "utils/Math.hpp"
#include <cmath>
#include <string>

namespace wf::analytics {

// one variant of an item: a mod rank, a fish size, a relic refinement...
struct Offer {
    Item item;
    std::string variant;
    double price = 0.0;
    double volume = 0.0;
    double score = 0.0;

    [[nodiscard]] json::Json toJson() const
    {
        return json::Json()
            .set("slug", this->item.slug)
            .set("name", this->item.name)
            .set("thumb", this->item.thumb)
            .set("variant", this->variant)
            .set("price", utils::roundTo(this->price, 1))
            .set("volume", utils::roundTo(this->volume, 1))
            .set("score", std::lround(this->score));
    }
};

}

#endif /* OFFER_HPP_ */
