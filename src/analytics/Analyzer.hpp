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
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wf::analytics {

class Analyzer {
    private:
        struct Days {
            int rank = 0;
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

        // item tag -> rank 0 copies needed to reach a rank
        static inline const std::map<std::string_view, std::function<double(int)>> COSTS{
            {"arcane_enhancement", [](int rank) { return (rank + 1) * (rank + 2) / 2.0; }},
        };

        [[nodiscard]] double _copies(const Item& item, int rank) const
        {
            for (const std::string& tag : item.tags) {
                const auto cost = this->COSTS.find(tag);

                if (cost != this->COSTS.end())
                    return cost->second(rank);
            }
            return 1.0;
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
                days.prices.push_back(day.at("median").get<double>());
                days.volumes.push_back(day.at("volume").get<double>());
            }
            return variants;
        }

    public:
        [[nodiscard]] std::vector<Offer> analyze(const Item& item, const json::Json& statistics) const
        {
            std::map<std::string, Days> variants = this->_group(statistics);
            std::vector<Offer> offers;

            if (variants.empty())
                variants[""];
            for (auto& [variant, days] : variants) {
                const double price = utils::median(days.prices);
                const double copies = this->_copies(item, days.rank);

                days.volumes.resize(std::max(days.volumes.size(), constants::PERIOD_DAYS));
                offers.push_back({
                    .item = item,
                    .variant = variant,
                    .price = price,
                    .volume = utils::trimmedMean(days.volumes, constants::VOLUME_TRIM),
                    .copies = copies,
                    .unitPrice = price / copies,
                });
            }
            return offers;
        }
};

}

#endif /* ANALYZER_HPP_ */
