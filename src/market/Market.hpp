/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Market.hpp
*/

#ifndef MARKET_HPP_
    #define MARKET_HPP_

#include "constants.hpp"
#include "curl/ACurl.hpp"
#include "json/Json.hpp"
#include "utils/RateLimiter.hpp"
#include <string>
#include <string_view>

namespace wf::market {

class Market : public curl::ACurl {
    private:
        utils::RateLimiter _limiter{constants::REQUESTS_PER_SECOND};

        [[nodiscard]] json::Json _fetch(std::string_view url)
        {
            this->_limiter.wait();
            this->curl(url);

            const long status = this->getStatusCode();

            if (status < 200 || status >= 300)
                throw MarketException(std::string(url) + " answered HTTP " + std::to_string(status));
            return json::Json::parse(this->getResponse());
        }

    public:
        Market(std::string_view language = constants::DEFAULT_LANGUAGE)
        {
            this->setOpt(CURLOPT_USERAGENT, constants::USER_AGENT);
            this->setHeader("Accept", "application/json");
            this->setHeader("Language", language);
        }

        [[nodiscard]] json::Json getItems()
        {
            return this->_fetch(constants::concat<constants::PRODUCTION_URL, constants::ITEMS>).at("data");
        }

        // exceptions
        class MarketException : public CurlException {
            public:
                explicit MarketException(const std::string& msg) : CurlException(msg) {}
        };
};

}

#endif /* MARKET_HPP_ */
