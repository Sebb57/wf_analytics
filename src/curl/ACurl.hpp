/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** ACurl.hpp
*/

#ifndef ACURL_HPP_
    #define ACURL_HPP_

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <curl/curl.h>
#include <exception>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace wf::curl {

class ACurl {
    public:
        enum class Method { Get, Post, Put, Patch, Delete, Head, Options };

    private:
        struct GlobalGuard {
            GlobalGuard()
            {
                if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
                    throw GlobalInitFail();
            }
            ~GlobalGuard() { curl_global_cleanup(); }
        };

        struct CaseInsensitiveLess {
            bool operator()(const std::string& lhs, const std::string& rhs) const
            {
                return std::lexicographical_compare(
                    lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                    [](unsigned char l, unsigned char r) { return std::tolower(l) < std::tolower(r); }
                );
            }
        };

        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> _curl;
        std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> _headerList{nullptr, curl_slist_free_all};
        std::map<std::string, std::string, CaseInsensitiveLess> _headers;
        std::string _response;

        static CURL* _initHandle()
        {
            static const GlobalGuard guard;
            CURL* handle = curl_easy_init();

            if (!handle)
                throw EasyInitFail();
            return handle;
        }
        static size_t _writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
        {
            const size_t totalSize = size * nmemb;

            try {
                static_cast<std::string*>(userp)->append(static_cast<char*>(contents), totalSize);
            } catch (const std::exception&) {
                return 0;
            }
            return totalSize;
        }
        const char* _methodName(Method method) const
        {
            switch (method) {
                case Method::Get: return "GET";
                case Method::Post: return "POST";
                case Method::Put: return "PUT";
                case Method::Patch: return "PATCH";
                case Method::Delete: return "DELETE";
                case Method::Head: return "HEAD";
                case Method::Options: return "OPTIONS";
            }
            return "GET";
        }
        void _initOpts()
        {
            this->setOpt(CURLOPT_WRITEFUNCTION, this->_writeCallback);
            this->setOpt(CURLOPT_WRITEDATA, &this->_response);
            this->setOpt(CURLOPT_FOLLOWLOCATION, 1L);
            this->setOpt(CURLOPT_CONNECTTIMEOUT, 10L);
            this->setOpt(CURLOPT_TIMEOUT, 30L);
        }
        void _applyMethod(Method method, std::string_view body)
        {
            this->setOpt(CURLOPT_HTTPGET, 1L);
            this->setOpt(CURLOPT_CUSTOMREQUEST, nullptr);

            if (method == Method::Get)
                return;
            if (method == Method::Head) {
                this->setOpt(CURLOPT_NOBODY, 1L);
                return;
            }
            if (method == Method::Post || !body.empty()) {
                this->setOpt(CURLOPT_POST, 1L);
                this->setOpt(CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.size()));
                this->setOpt(CURLOPT_COPYPOSTFIELDS, std::string(body));
            }
            if (method != Method::Post)
                this->setOpt(CURLOPT_CUSTOMREQUEST, this->_methodName(method));
        }
        void _applyHeaders()
        {
            curl_slist* list = nullptr;

            for (const auto& [name, value] : this->_headers) {
                const std::string line = value.empty() ? name + ";" : name + ": " + value;
                curl_slist* next = curl_slist_append(list, line.c_str());

                if (!next) {
                    curl_slist_free_all(list);
                    throw CurlException("Failed to build request headers");
                }
                list = next;
            }
            this->_headerList.reset(list);
            this->setOpt(CURLOPT_HTTPHEADER, list);
        }

    protected:
        ACurl() : _curl(this->_initHandle(), curl_easy_cleanup) { this->_initOpts(); }

        template <typename T>
        void setOpt(CURLoption opt, T param)
        {
            CURLcode code = CURLE_OK;

            if constexpr (std::is_same_v<T, std::string>) {
                code = curl_easy_setopt(this->_curl.get(), opt, param.c_str());
            } else if constexpr (std::is_same_v<T, std::string_view>) {
                code = curl_easy_setopt(this->_curl.get(), opt, std::string(param).c_str());
            } else if constexpr ((std::is_integral_v<T> || std::is_enum_v<T>) && !std::is_same_v<T, curl_off_t>) {
                code = curl_easy_setopt(this->_curl.get(), opt, static_cast<long>(param));
            } else {
                code = curl_easy_setopt(this->_curl.get(), opt, param);
            }
            if (code != CURLE_OK)
                throw CurlException(curl_easy_strerror(code));
        }
        void setHeader(std::string_view name, std::string_view value)
        {
            this->_headers[std::string(name)] = std::string(value);
        }
        void removeHeader(std::string_view name) { this->_headers.erase(std::string(name)); }
        void clearHeaders() { this->_headers.clear(); }

    public:
        ACurl(const ACurl&) = delete;
        virtual ~ACurl() = default;

        void curl(std::string_view url, Method method = Method::Get, std::string_view body = {})
        {
            this->_response.clear();
            this->setOpt(CURLOPT_URL, url);
            this->_applyMethod(method, body);
            this->_applyHeaders();

            const CURLcode code = curl_easy_perform(this->_curl.get());

            if (code != CURLE_OK)
                throw CurlException(std::string(curl_easy_strerror(code)) + " (" + std::string(url) + ")");
        }
        [[nodiscard]] const std::string& getResponse() const { return this->_response; }
        [[nodiscard]] long getStatusCode() const { return this->getInfo<long>(CURLINFO_RESPONSE_CODE); }
        [[nodiscard]] std::optional<std::string> getResponseHeader(const std::string& name) const
        {
            curl_header* header = nullptr;

            if (curl_easy_header(this->_curl.get(), name.c_str(), 0, CURLH_HEADER, -1, &header) != CURLHE_OK)
                return std::nullopt;
            return std::string(header->value);
        }
        template <typename T>
        [[nodiscard]] T getInfo(CURLINFO info) const
        {
            T value{};
            const CURLcode code = curl_easy_getinfo(this->_curl.get(), info, &value);

            if (code != CURLE_OK)
                throw CurlException(curl_easy_strerror(code));
            return value;
        }
        [[nodiscard]] std::string escape(std::string_view text) const
        {
            if (text.empty())
                return {};
            const std::unique_ptr<char, decltype(&curl_free)> escaped(
                curl_easy_escape(this->_curl.get(), text.data(), static_cast<int>(text.size())),
                curl_free
            );

            if (!escaped)
                throw CurlException("Failed to escape url");
            return escaped.get();
        }

        // exceptions
        class CurlException : public std::exception {
            protected:
                std::string _msg;

            public:
                explicit CurlException(const std::string& msg) : _msg(msg) {}
                const char* what() const noexcept override { return this->_msg.c_str(); }
        };

        class GlobalInitFail : public CurlException {
            public:
                GlobalInitFail() : CurlException("Failed to init curl global") {}
        };

        class EasyInitFail : public CurlException {
            public:
                EasyInitFail() : CurlException("Failed to init curl easy") {}
        };
};

}

#endif /* ACURL_HPP_ */
