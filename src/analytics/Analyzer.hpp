/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Analyzer.hpp
*/

#ifndef ANALYZER_HPP_
    #define ANALYZER_HPP_

#include "analytics/Item.hpp"
#include "analytics/Offer.hpp"
#include "constants.hpp"
#include "json/Json.hpp"
#include "utils/Math.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wf::analytics {

class Analyzer {
    private:
        struct Days {
            int rank = 0;
            bool dropped = true;    // the form of the item that drops in missions
            std::vector<double> prices;
            std::vector<double> volumes;
        };

        // statistics field -> label
        static constexpr std::array<std::pair<std::string_view, std::string_view>, 4> VARIANTS{{
            {"mod_rank", "rank "},
            {"subtype", ""},
            {"cyan_stars", "cyan "},
            {"amber_stars", "amber "},
        }};

        // statistics field and value of the forms that drop: anything else is upgraded or refined
        static inline const std::set<std::pair<std::string_view, std::string_view>> DROPPED{
            {"mod_rank", "0"},
            {"subtype", "intact"},
            {"subtype", "regular"},
            {"cyan_stars", "0"},
            {"amber_stars", "0"},
        };

        // item tag -> rank 0 copies needed to reach a rank
        static inline const std::map<std::string_view, std::function<double(int)>> COSTS{
            {"arcane_enhancement", [](int rank) { return (rank + 1) * (rank + 2) / 2.0; }},
        };

        // nothing when the item has no cost rule
        [[nodiscard]] std::optional<double> _copies(const Item& item, int rank) const
        {
            for (const std::string& tag : item.tags) {
                const auto cost = this->COSTS.find(tag);

                if (cost != this->COSTS.end())
                    return cost->second(rank);
            }
            return std::nullopt;
        }
        [[nodiscard]] bool _dropped(const json::Json& day) const
        {
            for (const auto& [field, label] : this->VARIANTS) {
                if (!day.contains(field))
                    continue;
                const std::string value = day.at(field).text();

                if (!this->DROPPED.contains({field, value}))
                    return false;
            }
            return true;
        }
        [[nodiscard]] std::string _variant(const json::Json& day) const
        {
            std::string variant;

            for (const auto& [field, label] : this->VARIANTS) {
                if (!day.contains(field))
                    continue;
                variant += (variant.empty() ? "" : ", ") + std::string(label) + day.at(field).text();
            }
            return variant;
        }
        [[nodiscard]] std::map<std::string, Days> _group(const json::Json& statistics) const
        {
            std::map<std::string, Days> variants;

            for (const json::Json& day : statistics.at("statistics_closed").at(constants::PERIOD).elements()) {
                Days& days = variants[this->_variant(day)];

                days.rank = day.contains("mod_rank") ? day.at("mod_rank").get<int>() : 0;
                days.dropped = this->_dropped(day);
                days.prices.push_back(day.at("median").get<double>());
                days.volumes.push_back(day.at("volume").get<double>());
            }
            return variants;
        }

    public:
        // farmHours: to farm one copy of the item as it drops
        [[nodiscard]] std::vector<Offer> analyze(const Item& item, const json::Json& statistics, std::optional<double> farmHours) const
        {
            std::map<std::string, Days> variants = this->_group(statistics);
            std::vector<Offer> offers;

            if (variants.empty())
                variants[""];
            for (auto& [variant, days] : variants) {
                const double price = utils::median(days.prices);
                const std::optional<double> built = this->_copies(item, days.rank);
                const double copies = built.value_or(1.0);
                // farm counts for what drops, and for what is built from it
                const std::optional<double> hours = farmHours && (days.dropped || built) ? std::optional<double>(*farmHours * copies) : std::nullopt;

                days.volumes.resize(std::max(days.volumes.size(), constants::PERIOD_DAYS));
                offers.push_back({
                    .item = item,
                    .variant = variant,
                    .price = price,
                    .volume = utils::trimmedMean(days.volumes, constants::VOLUME_TRIM),
                    .copies = copies,
                    .unitPrice = price / copies,
                    .farmHours = hours,
                    .platPerHour = hours ? std::optional<double>(price / *hours) : std::nullopt,
                    .farmRate = hours ? std::optional<double>(copies / *hours) : std::nullopt,
                });
            }
            return offers;
        }
};

}

#endif /* ANALYZER_HPP_ */
