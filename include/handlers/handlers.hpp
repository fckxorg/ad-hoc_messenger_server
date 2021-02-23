#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <crow_all.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <chrono>
#include <ctime>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <sstream>

#include "request_helpers.hpp"

#define THROW_RESPONSE(condition, code) \
    if ((condition)) {                  \
        return crow::response(code);    \
    }

#define THROW_BAD_REQUEST_IF(condition) THROW_RESPONSE((condition), 400)

#define THROW_NOT_FOUND_IF(condition) THROW_RESPONSE((condition), 404)

crow::response user_find_handler(const crow::request& req,
                                 const mongocxx::database& db);
crow::response key_update_handler(const crow::request& req,
                                  const mongocxx::database& db);
crow::response message_send_handler(const crow::request& req,
                                    const mongocxx::database& db);
#endif
