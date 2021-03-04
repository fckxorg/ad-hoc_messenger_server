#include "request_helpers.hpp"

std::optional<std::chrono::system_clock::time_point> str_to_tp(
    const std::string& time_string) {
    std::istringstream time_sent(time_string);
    std::tm parsed_time = {};
    time_sent >> std::get_time(&parsed_time, "%Y-%m-%d %H:%M:%S");

    if (time_sent.fail()) {
        return std::nullopt;
    }

    std::time_t utc_time = std::mktime(&parsed_time);

    if (-1 == utc_time) {
        return std::nullopt;
    }

    auto tp = std::chrono::system_clock::from_time_t(utc_time);

    return std::optional<decltype(tp)>(tp);
}
