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
#include <map>
#include <optional>
#include <string>

namespace wf::analytics {

// one variant of an item: a mod rank, a fish size, a relic refinement...
struct Offer {
    Item item;
    std::string variant;
    double price = 0.0;
    double volume = 0.0;
    double copies = 1.0;        // rank 0 copies used to make one
    double unitPrice = 0.0;     // price of one of those copies
    std::optional<double> farmHours;     // to farm what makes one, nothing if it does not drop
    std::optional<double> platPerHour;   // price earned per hour of farm
    std::optional<double> farmRate;      // rank 0 copies farmed per hour
    std::map<std::string, double> scores{};     // ranking -> score out of 100

    [[nodiscard]] json::Json toJson() const
    {
        json::Json scores;

        for (const auto& [ranking, score] : this->scores)
            scores.set(ranking, std::lround(score));

        json::Json offer = json::Json()
            .set("slug", this->item.slug)
            .set("name", this->item.name)
            .set("thumb", this->item.thumb)
            .set("variant", this->variant)
            .set("price", utils::roundTo(this->price, 1))
            .set("volume", utils::roundTo(this->volume, 1))
            .set("copies", std::lround(this->copies))
            .set("scores", scores);

        if (this->farmHours && this->platPerHour)
            offer.set("farm", utils::roundTo(*this->farmHours, 2)).set("perHour", utils::roundTo(*this->platPerHour, 1));
        return offer;
    }
};

}

#endif /* OFFER_HPP_ */
