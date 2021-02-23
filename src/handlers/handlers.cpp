#include "handlers.hpp"

#include <crow_all.h>

#include <chrono>
#include <ctime>
#include <sstream>

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

crow::response user_find_handler(const crow::request& req,
                                 const mongocxx::database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    if (!ValidateRequest(request, "handle")) {
        return crow::response(400);
    }

    bsoncxx::document::value filter_document =
        make_document(kvp("handle", request["handle"].s()));

    mongocxx::collection users = db["users"];
    auto maybe_result = users.find_one(std::move(filter_document));

    if (!maybe_result) {
        return crow::response(404);
    }

    bsoncxx::document::value found_user = *maybe_result;
    crow::json::wvalue response_body{};

    response_body["public_key"] =
        bsoncxx::stdx::string_view(found_user.view()["public_key"].get_utf8())
            .data();

    return crow::response(200, response_body.dump());
}

crow::response key_update_handler(const crow::request& req,
                                  const mongocxx::database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    if (!ValidateRequest(request, "public_key", "handle")) {
        return crow::response(400);
    }

    mongocxx::collection users = db["users"];

    bsoncxx::document::value filter =
        make_document(kvp("handle", request["handle"].s()));
    bsoncxx::document::value update = make_document(kvp(
        "$set", make_document(kvp("public_key", request["public_key"].s()))));

    auto maybe_result =
        users.find_one_and_update(std::move(filter), std::move(update));

    if (!maybe_result) {
        return crow::response(404);
    }

    return crow::response(200);
}

// time format in json: %Y-%m-%d %H:%M:%S
crow::response message_send_handler(const crow::request& req,
                                    const mongocxx::database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    if (!ValidateRequest(request, "sender", "reciever", "payload", "datetime",
                         "encrypted_by")) {
        return crow::response(400);
    }

    std::istringstream time_sent(request["datetime"]);
    std::tm parsed_time = {};

    time_sent >> std::get_time(&t, "%Y-$m-%d %H:%M:%S");

    if (time_sent.fail()) {
        return crow::response(400);
    }

    std::time_t utc_time = std::mktime(&parsed_time);
    if (-1 == utc_time) {
        return crow::response(400);
    }

    bsoncxx::types::b_date bson_date{
        std::chrono::system_clock::from_time_t(utc_time)};

    // TODO check sender and reciever are both exist

    bsoncxx::document::value new_message = make_document(
        kvp("sender", request["sender"]), kvp("reciever", request["reciever"]),
        kvp("payload", request["payload"]),
        kvp("encrypted_by", request["encrypted_by"]),
        kvp("datetime", bson_date));

    mongocxx::collection messages = db["messages"];

    messages.insert_one(new_message);
}
