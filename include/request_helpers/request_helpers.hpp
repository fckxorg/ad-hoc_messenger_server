#ifndef REQUEST_HELPERS_HPP
#define REQUEST_HELPERS_HPP

#include <crow_all.h>

#include <chrono>
#include <optional>
#include <sstream>
#include <string>

template <typename... Args>
bool ValidateFold(Args... args) {
    return (args && ...);
}

template <typename... Args>
bool ValidateRequest(const crow::json::rvalue& request, Args... args) {
    return request && ValidateFold(request.has(args)...);
}

std::optional<std::chrono::system_clock::time_point> str_to_tp(
    const std::string& time_string);

std::string tp_to_str(const std::chrono::system_clock::time_point& tp);

#endif
