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
#include "utils/Math.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <vector>

namespace wf::analytics {

class Scorer {
    private:
        static constexpr std::array<double Offer::*, 2> CRITERIA{&Offer::price, &Offer::volume};

        [[nodiscard]] static std::vector<double> _sorted(const std::vector<Offer>& offers, double Offer::* criterion)
        {
            std::vector<double> values;

            for (const Offer& offer : offers)
                values.push_back(offer.*criterion);
            std::ranges::sort(values);
            return values;
        }

    public:
        // score = geometric mean of the percentile rank of each criterion
        static void rank(std::vector<Offer>& offers)
        {
            for (Offer& offer : offers)
                offer.score = 1.0;
            for (const auto criterion : CRITERIA) {
                const std::vector<double> sorted = _sorted(offers, criterion);

                for (Offer& offer : offers)
                    offer.score *= utils::percentileRank(sorted, offer.*criterion);
            }
            for (Offer& offer : offers)
                offer.score = constants::MAX_SCORE * std::pow(offer.score, 1.0 / CRITERIA.size());
            std::ranges::sort(offers, std::greater{}, &Offer::score);
        }
};

}

#endif /* SCORER_HPP_ */
