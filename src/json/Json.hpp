/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** Json.hpp
*/

#ifndef JSON_HPP_
    #define JSON_HPP_

#include <cstddef>
#include <exception>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace wf::json {

class Json {
    private:
        nlohmann::json _data;

        explicit Json(nlohmann::json data) : _data(std::move(data)) {}

        template <typename Operation>
        static auto _guard(Operation&& operation)
        {
            try {
                return operation();
            } catch (const nlohmann::json::exception& e) {
                throw JsonException(e.what());
            }
        }

    public:
        Json() = default;

        template <typename T>
        [[nodiscard]] T get() const { return _guard([&] { return this->_data.template get<T>(); }); }
        [[nodiscard]] static Json parse(std::string_view text) { return _guard([&] { return Json(nlohmann::json::parse(text)); }); }
        [[nodiscard]] Json at(std::string_view key) const { return _guard([&] { return Json(this->_data.at(std::string(key))); }); }
        [[nodiscard]] Json at(std::size_t index) const { return _guard([&] { return Json(this->_data.at(index)); }); }
        [[nodiscard]] bool contains(std::string_view key) const { return this->_data.contains(std::string(key)); }
        [[nodiscard]] bool isNull() const { return this->_data.is_null(); }
        [[nodiscard]] bool isObject() const { return this->_data.is_object(); }
        [[nodiscard]] bool isArray() const { return this->_data.is_array(); }
        [[nodiscard]] std::size_t size() const { return this->_data.size(); }
        [[nodiscard]] std::string dump(int indent = -1) const { return _guard([&] { return this->_data.dump(indent); }); }

        // exceptions
        class JsonException : public std::exception {
            private:
                std::string _msg;

            public:
                explicit JsonException(const std::string& msg) : _msg(msg) {}
                const char* what() const noexcept override { return this->_msg.c_str(); }
        };
};

}

#endif /* JSON_HPP_ */
