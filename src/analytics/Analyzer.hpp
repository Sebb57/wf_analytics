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
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wf::analytics {

class Analyzer {
    private:
        struct Days {
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

        [[nodiscard]] static std::string _variant(const json::Json& day)
        {
            std::string variant;

            for (const auto& [field, label] : VARIANTS) {
                if (!day.contains(field))
                    continue;
                variant += (variant.empty() ? "" : ", ") + std::string(label) + day.at(field).text();
            }
            return variant;
        }
        [[nodiscard]] static std::map<std::string, Days> _group(const json::Json& statistics)
        {
            std::map<std::string, Days> variants;

            for (const json::Json& day : statistics.at("statistics_closed").at(constants::PERIOD).elements()) {
                Days& days = variants[_variant(day)];

                days.prices.push_back(day.at("median").get<double>());
                days.volumes.push_back(day.at("volume").get<double>());
            }
            return variants;
        }

    public:
        [[nodiscard]] static std::vector<Offer> analyze(const Item& item, const json::Json& statistics)
        {
            std::map<std::string, Days> variants = _group(statistics);
            std::vector<Offer> offers;

            if (variants.empty())
                variants[""];
            for (auto& [variant, days] : variants) {
                days.volumes.resize(std::max(days.volumes.size(), constants::PERIOD_DAYS));
                offers.push_back({item, variant, utils::median(days.prices), utils::trimmedMean(days.volumes, constants::VOLUME_TRIM)});
            }
            return offers;
        }
};

}

#endif /* ANALYZER_HPP_ */
