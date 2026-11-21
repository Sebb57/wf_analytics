/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Market.hpp
*/

#ifndef MARKET_HPP_
    #define MARKET_HPP_

#include "api/AJsonApi.hpp"
#include "constants.hpp"
#include "json/Json.hpp"
#include <format>
#include <string_view>

namespace wf::market {

class Market : public api::AJsonApi {
    public:
        Market(std::string_view language = constants::DEFAULT_LANGUAGE) : AJsonApi(constants::REQUESTS_PER_SECOND)
        {
            this->setHeader("Language", language);
        }

        [[nodiscard]] json::Json getItems()
        {
            return this->fetch(constants::concat<constants::PRODUCTION_URL, constants::ITEMS>).at("data");
        }
        [[nodiscard]] json::Json getStatistics(std::string_view slug)
        {
            return this->fetch(std::format("{}{}{}{}", constants::OLD_URL, constants::OLD_ITEM, this->escape(slug), constants::OLD_STATISTICS)).at("payload");
        }
};

}

#endif /* MARKET_HPP_ */
