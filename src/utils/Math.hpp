/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Math.hpp
*/

#ifndef MATH_HPP_
    #define MATH_HPP_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <utility>
#include <vector>

namespace wf::utils {

// mean once the lowest and highest `trim` share of the values are dropped
[[nodiscard]] inline double trimmedMean(std::vector<double> values, double trim)
{
    if (values.empty())
        return 0.0;
    std::ranges::sort(values);

    const std::size_t cut = std::min(static_cast<std::size_t>(values.size() * trim), (values.size() - 1) / 2);
    const auto first = values.begin() + cut;
    const auto last = values.end() - cut;

    return std::accumulate(first, last, 0.0) / static_cast<double>(last - first);
}

[[nodiscard]] inline double median(std::vector<double> values) { return trimmedMean(std::move(values), 0.5); }

// share of the sorted values that are below value, from 0 to 1
[[nodiscard]] inline double percentileRank(const std::vector<double>& sorted, double value)
{
    if (sorted.empty())
        return 0.0;
    return static_cast<double>(std::ranges::lower_bound(sorted, value) - sorted.begin()) / static_cast<double>(sorted.size());
}

[[nodiscard]] inline double roundTo(double value, int decimals)
{
    const double factor = std::pow(10.0, decimals);

    return std::round(value * factor) / factor;
}

}

#endif /* MATH_HPP_ */
