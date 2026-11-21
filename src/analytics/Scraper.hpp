/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Scraper.hpp
*/

#ifndef SCRAPER_HPP_
    #define SCRAPER_HPP_

#include "analytics/Analyzer.hpp"
#include "analytics/Farm.hpp"
#include "analytics/Item.hpp"
#include "analytics/Offer.hpp"
#include "analytics/Scorer.hpp"
#include "drops/Drops.hpp"
#include "json/Json.hpp"
#include "market/Market.hpp"
#include <chrono>
#include <cstddef>
#include <exception>
#include <format>
#include <iostream>
#include <vector>

namespace wf::analytics {

class Scraper {
    private:
        market::Market _market;
        Farm _farm{drops::Drops().getDrops()};
        Analyzer _analyzer;
        Scorer _scorer;
        std::vector<Offer> _offers;

        // what has to be farmed to make the item: its parts for a set, itself otherwise
        [[nodiscard]] std::vector<Farm::Part> _parts(const Item& item)
        {
            std::vector<Farm::Part> parts;

            if (!item.hasTag("set"))
                return {{item.name}};
            for (const json::Json& part : this->_market.getSet(item.slug).elements()) {
                if (!part.at("setRoot").get<bool>())
                    parts.push_back({Item::fromJson(part).name, part.value("quantityInSet", 1.0)});
            }
            return parts;
        }
        void _scrape(const Item& item)
        {
            const std::optional<double> farmHours = this->_farm.hours(this->_parts(item));
            const std::vector<Offer> offers = this->_analyzer.analyze(item, this->_market.getStatistics(item.slug), farmHours);

            this->_offers.insert(this->_offers.end(), offers.begin(), offers.end());
        }

    public:
        void run()
        {
            const std::vector<json::Json> items = this->_market.getItems().elements();

            for (std::size_t i = 0; i < items.size(); i++) {
                const Item item = Item::fromJson(items[i]);

                std::cerr << "\r[" << i + 1 << "/" << items.size() << "] " << std::flush;
                try {
                    this->_scrape(item);
                } catch (const std::exception& e) {
                    std::cerr << item.slug << ": " << e.what() << std::endl;
                }
            }
            std::cerr << std::endl;
            this->_scorer.rank(this->_offers);
        }
        [[nodiscard]] json::Json toJson() const
        {
            json::Json offers = json::Json::array();

            for (const Offer& offer : this->_offers)
                offers.push(offer.toJson());
            return json::Json()
                .set("updated", std::format("{:%FT%TZ}", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())))
                .set("offers", offers);
        }
};

}

#endif /* SCRAPER_HPP_ */
