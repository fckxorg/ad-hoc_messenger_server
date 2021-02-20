#ifndef REQUEST_HELPERS_HPP
#define REQUEST_HELPERS_HPP

#include <crow_all.h>

template <typename... Args>
bool ValidateFold(Args... args) {
    return (args && ...);
}

template <typename... Args>
bool ValidateRequest(const crow::json::rvalue& request, Args... args) {
    return request && ValidateFold(request.has(args)...);
}

#endif
