#include "handlers.hpp"

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

crow::response user_find_handler(const crow::request& req,
                                 const mongocxx::database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    THROW_BAD_REQUEST_IF(!ValidateRequest(request, "handle"));

    mongocxx::collection users = db["users"];
    bsoncxx::document::value filter_document =
        make_document(kvp("handle", request["handle"].s()));
    auto maybe_result = users.find_one(std::move(filter_document));

    THROW_NOT_FOUND_IF(!maybe_result);

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

    THROW_BAD_REQUEST_IF(!ValidateRequest(request, "public_key", "handle"));

    mongocxx::collection users = db["users"];
    bsoncxx::document::value filter =
        make_document(kvp("handle", request["handle"].s()));
    bsoncxx::document::value update = make_document(kvp(
        "$set", make_document(kvp("public_key", request["public_key"].s()))));

    auto maybe_result =
        users.find_one_and_update(std::move(filter), std::move(update));

    THROW_NOT_FOUND_IF(!maybe_result);

    return crow::response(200);
}

// time format in json: %Y-%m-%d %H:%M:%S
crow::response message_send_handler(const crow::request& req,
                                    const mongocxx::database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    THROW_BAD_REQUEST_IF(!ValidateRequest(
        request, "sender", "reciever", "payload", "datetime", "encrypted_by"));

    std::istringstream time_sent(request["datetime"].s());
    std::tm parsed_time = {};
    time_sent >> std::get_time(&parsed_time, "%Y-$m-%d %H:%M:%S");

    THROW_BAD_REQUEST_IF(time_sent.fail());

    std::time_t utc_time = std::mktime(&parsed_time);
    THROW_BAD_REQUEST_IF(-1 == utc_time);

    bsoncxx::types::b_date bson_date{
        std::chrono::system_clock::from_time_t(utc_time)};

    // TODO check sender and reciever are both exist

    bsoncxx::document::value new_message =
        make_document(kvp("sender", request["sender"].s()),
                      kvp("reciever", request["reciever"].s()),
                      kvp("payload", request["payload"].s()),
                      kvp("encrypted_by", request["encrypted_by"].s()),
                      kvp("datetime", bson_date));

    mongocxx::collection messages = db["messages"];

    messages.insert_one(std::move(new_message));

    return crow::response(200);
}
