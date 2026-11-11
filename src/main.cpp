#include "analytics/Scraper.hpp"
#include "constants.hpp"
#include <exception>
#include <iostream>

int main()
{
    try {
        wf::analytics::Scraper scraper;

        scraper.run();
        std::cout << scraper.toJson().dump() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return wf::constants::FAILURE;
    }
    return wf::constants::SUCCESS;
}
