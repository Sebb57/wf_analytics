/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Farm.hpp
*/

#ifndef FARM_HPP_
    #define FARM_HPP_

#include "json/Json.hpp"
#include "utils/String.hpp"
#include <algorithm>
#include <array>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wf::analytics {

// expected time to farm an item, from its drop chances and assumed mission times
class Farm {
    public:
        struct Part {
            std::string name;
            double quantity = 1.0;
        };

    private:
        struct Source {
            std::string place;
            double chance = 0.0;    // from 0 to 1
        };

        struct Mission {
            std::string_view keyword;   // found in the drop place
            double minutes;             // for one reward
            bool endless;               // rewards follow the A, A, B, C rotation cycle
        };

        // first match wins: keep the specific keywords before the generic ones
        static constexpr std::array<Mission, 32> MISSIONS{{
            {"Eidolon", 15.0, false},
            {"Bounty)", 10.0, false},
            {"Isolation Vault)", 12.0, false},
            {"PROFIT-TAKER", 10.0, false},
            {"Derelict Vault", 10.0, false},
            {"Endless: Tier", 8.0, false},
            {"(Skirmish)", 12.0, false},
            {"(Capture)", 3.0, false},
            {"(Exterminate)", 4.0, false},
            {"(Rescue)", 4.0, false},
            {"(Sabotage)", 6.0, false},
            {"(Caches)", 8.0, false},
            {"(Spy)", 6.0, false},
            {"(Rush)", 5.0, false},
            {"(Mobile Defense)", 5.0, false},
            {"(Assassination)", 6.0, false},
            {"(Hijack)", 6.0, false},
            {"(Survival)", 5.0, true},
            {"(Conjunction Survival)", 5.0, true},
            {"(Defense)", 3.0, true},
            {"(Mirror Defense)", 3.0, true},
            {"(Interception)", 3.0, true},
            {"(Excavation)", 3.0, true},
            {"(Disruption)", 3.0, true},
            {"(Defection)", 4.0, true},
            {"(Infested Salvage)", 4.0, true},
            {"(Alchemy)", 4.0, true},
            {"(Void Flood)", 4.0, true},
            {"(Void Cascade)", 4.0, true},
            {"(Void Armageddon)", 4.0, true},
            {"(The Circuit)", 5.0, true},
            {"Arbitrations", 5.0, true},
        }};

        // rotation -> rewards of the A, A, B, C cycle spent to get one of it
        static constexpr std::array<std::pair<std::string_view, double>, 3> ROTATIONS{{
            {"Rotation A", 2.0},
            {"Rotation B", 4.0},
            {"Rotation C", 4.0},
        }};

        // to open a relic in a void fissure
        static constexpr double OPEN_MINUTES = 5.0;

        std::map<std::string, std::vector<Source>> _sources;        // lowercase item -> where it drops
        std::map<std::string, std::optional<double>> _minutes;     // lowercase item -> best farm time

        // minutes for one reward of a mission, nothing if the place is not a known mission
        [[nodiscard]] std::optional<double> _mission(const std::string& place) const
        {
            const auto mission = std::ranges::find_if(this->MISSIONS, [&](const Mission& known) { return place.find(known.keyword) != std::string::npos; });

            if (mission == this->MISSIONS.end())
                return std::nullopt;
            if (!mission->endless)
                return mission->minutes;
            const auto rotation = std::ranges::find_if(this->ROTATIONS, [&](const auto& known) { return place.find(known.first) != std::string::npos; });

            return mission->minutes * (rotation == this->ROTATIONS.end() ? 1.0 : rotation->second);
        }
        // minutes for one try at a place: a mission, or an item that drops itself (a relic) to farm then open
        [[nodiscard]] std::optional<double> _try(const std::string& place)
        {
            if (!this->_sources.contains(utils::lower(place)))
                return this->_mission(place);
            const std::optional<double> container = this->_farm(place);

            return container ? std::optional<double>(*container + this->OPEN_MINUTES) : std::nullopt;
        }
        // expected minutes to get one, from its best source
        [[nodiscard]] std::optional<double> _farm(const std::string& name)
        {
            const std::string key = utils::lower(name);
            const auto known = this->_minutes.find(key);
            const auto sources = this->_sources.find(key);
            std::optional<double> best;

            if (known != this->_minutes.end())
                return known->second;
            if (sources == this->_sources.end())
                return std::nullopt;
            // a place leading back to this item is not a way to farm it
            this->_minutes[key] = std::nullopt;
            for (const Source& source : sources->second) {
                const std::optional<double> minutes = this->_try(source.place);

                if (minutes && (!best || *minutes / source.chance < *best))
                    best = *minutes / source.chance;
            }
            this->_minutes[key] = best;
            return best;
        }

    public:
        explicit Farm(const json::Json& drops)
        {
            for (const json::Json& drop : drops.elements()) {
                const std::optional<double> percent = utils::toDouble(drop.at("chance").text());

                if (!percent || *percent <= 0.0)
                    continue;
                this->_sources[utils::lower(drop.at("item").text())].push_back({
                    utils::erase(utils::erase(drop.at("place").text(), "<b>"), "</b>"),
                    *percent / 100.0,
                });
            }
        }

        // hours to farm every part, nothing if one of them does not drop
        [[nodiscard]] std::optional<double> hours(const std::vector<Part>& parts)
        {
            double minutes = 0.0;

            if (parts.empty())
                return std::nullopt;
            for (const Part& part : parts) {
                const std::optional<double> one = this->_farm(part.name);

                if (!one)
                    return std::nullopt;
                minutes += *one * part.quantity;
            }
            return minutes / 60.0;
        }
};

}

#endif /* FARM_HPP_ */
