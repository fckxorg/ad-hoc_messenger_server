#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <crow_all.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include <chrono>
#include <ctime>
#include <sstream>

#include "request_helpers.hpp"

crow::response user_find_handler(const crow::request& req,
                                 const mongocxx::database& db);
crow::response key_update_handler(const crow::request& req,
                                  const mongocxx::database& db);
crow::response message_send_handler(const crow::request& req,
                                    const mongocxx::database& db);
#endif
