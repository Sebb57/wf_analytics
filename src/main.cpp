#include "constants.hpp"
#include "market/Market.hpp"
#include <exception>
#include <iostream>

int main()
{
    try {
        wf::market::Market market;
        const wf::json::Json items = market.getItems();

        std::cerr << "HTTP status: " << market.getStatusCode() << std::endl;
        std::cerr << "Items: " << items.size() << std::endl;
        std::cout << items.dump(4) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return wf::constants::FAILURE;
    }
    return wf::constants::SUCCESS;
}
