/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** AJsonApi.hpp
*/

#ifndef AJSONAPI_HPP_
    #define AJSONAPI_HPP_

#include "constants.hpp"
#include "curl/ACurl.hpp"
#include "json/Json.hpp"
#include "utils/RateLimiter.hpp"
#include <string>
#include <string_view>

namespace wf::api {

// client of a json api: rate limited requests whose answer is parsed
class AJsonApi : public curl::ACurl {
    private:
        utils::RateLimiter _limiter;

    protected:
        AJsonApi(double requestsPerSecond) : _limiter(requestsPerSecond)
        {
            this->setOpt(CURLOPT_USERAGENT, constants::USER_AGENT);
            this->setOpt(CURLOPT_ACCEPT_ENCODING, "");
            this->setHeader("Accept", "application/json");
        }

        // GET url and parse the body, throws unless the status is 2xx
        [[nodiscard]] json::Json fetch(std::string_view url)
        {
            this->_limiter.wait();
            this->curl(url);

            const long status = this->getStatusCode();

            if (status < 200 || status >= 300)
                throw ApiException(std::string(url) + " answered HTTP " + std::to_string(status));
            return json::Json::parse(this->getResponse());
        }

    public:
        // exceptions
        class ApiException : public CurlException {
            public:
                explicit ApiException(const std::string& msg) : CurlException(msg) {}
        };
};

}

#endif /* AJSONAPI_HPP_ */
