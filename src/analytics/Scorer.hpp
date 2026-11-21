/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Scorer.hpp
*/

#ifndef SCORER_HPP_
    #define SCORER_HPP_

#include "analytics/Offer.hpp"
#include "constants.hpp"
#include "json/Json.hpp"
#include "utils/Math.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace wf::analytics {

// value of an offer to rank on, nothing when it is unknown
using Criterion = std::optional<double> (*)(const Offer&);

template <auto Field>
[[nodiscard]] std::optional<double> criterion(const Offer& offer) { return offer.*Field; }

class Scorer {
    private:
        struct Ranking {
            std::string_view id;
            std::string_view label;
            std::vector<Criterion> criteria;
        };

        // the first one is the default ranking
        static inline const std::vector<Ranking> RANKINGS{
            {"farm", "Best to farm (platinum per hour)", {criterion<&Offer::platPerHour>, criterion<&Offer::volume>}},
            {"market", "Best to sell (farm ignored)", {criterion<&Offer::unitPrice>, criterion<&Offer::volume>}},
        };

        [[nodiscard]] std::vector<double> _sorted(const std::vector<Offer*>& offers, Criterion criterion) const
        {
            std::vector<double> values;

            for (const Offer* offer : offers)
                values.push_back(*criterion(*offer));
            std::ranges::sort(values);
            return values;
        }
        // score = geometric mean of the percentile rank of each criterion,
        // among the offers where every criterion is known
        void _rank(std::vector<Offer>& offers, const Ranking& ranking) const
        {
            const std::string id(ranking.id);
            std::vector<Offer*> ranked;

            for (Offer& offer : offers) {
                if (std::ranges::all_of(ranking.criteria, [&](Criterion criterion) { return criterion(offer).has_value(); }))
                    ranked.push_back(&offer);
            }
            for (Offer* offer : ranked)
                offer->scores[id] = 1.0;
            for (const Criterion criterion : ranking.criteria) {
                const std::vector<double> sorted = this->_sorted(ranked, criterion);

                for (Offer* offer : ranked)
                    offer->scores[id] *= utils::percentileRank(sorted, *criterion(*offer));
            }
            for (Offer* offer : ranked)
                offer->scores[id] = constants::MAX_SCORE * std::pow(offer->scores[id], 1.0 / ranking.criteria.size());
        }

    public:
        // scores every offer in every ranking, then sorts them on the default one
        void rank(std::vector<Offer>& offers) const
        {
            const std::string first(this->RANKINGS.front().id);

            for (const Ranking& ranking : this->RANKINGS)
                this->_rank(offers, ranking);
            std::ranges::sort(offers, std::greater{}, [&](const Offer& offer) { return offer.scores.contains(first) ? offer.scores.at(first) : -1.0; });
        }
        // the rankings, for the page to offer them
        [[nodiscard]] json::Json toJson() const
        {
            json::Json rankings = json::Json::array();

            for (const Ranking& ranking : this->RANKINGS)
                rankings.push(json::Json().set("id", std::string(ranking.id)).set("label", std::string(ranking.label)));
            return rankings;
        }
};

}

#endif /* SCORER_HPP_ */
