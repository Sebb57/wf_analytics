/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Drops.hpp
*/

#ifndef DROPS_HPP_
    #define DROPS_HPP_

#include "api/AJsonApi.hpp"
#include "constants.hpp"
#include "json/Json.hpp"

namespace wf::drops {

class Drops : public api::AJsonApi {
    public:
        Drops() : AJsonApi(constants::REQUESTS_PER_SECOND) {}

        // every line of the drop tables: { place, item, rarity, chance in percent }
        [[nodiscard]] json::Json getDrops() { return this->fetch(constants::DROPS_URL); }
};

}

#endif /* DROPS_HPP_ */
