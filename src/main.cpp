#include "constants.hpp"
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;

    auto* response = static_cast<std::string*>(userp);
    response->append(
        static_cast<char*>(contents),
        totalSize
    );

    return totalSize;
}

int main()
{
    CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (result != CURLE_OK)
        return wf::constants::FAILURE;

    CURL* curl = curl_easy_init();
    if (!curl) {
        curl_global_cleanup();
        return wf::constants::FAILURE;
    }

    std::string response;
    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Language: fr");

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.warframe.market/v2/items");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "MyWarframeApp/1.0 (C++ client)");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(result) << '\n';
    }
    else {
        long httpCode = 0;

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        std::cout << "HTTP status: " << httpCode << '\n';
        nlohmann::json result = nlohmann::json::parse(response);
        std::cout << result.dump(4) << std::endl;

    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return wf::constants::SUCCESS;
}
