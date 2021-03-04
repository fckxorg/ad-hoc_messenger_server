#include "handlers.hpp"

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

crow::response user_find_handler(const crow::request& req, Database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    THROW_BAD_REQUEST_IF(!ValidateRequest(request, "handle"),
                         "Invalid JSON format");

    auto query_result = db.get_collection<User>("users").filter_str_eq(
        {{"handle", request["handle"].s()}});
    // auto user_optional = find_user_by_handle(request["handle"].s(), db);

    THROW_NOT_FOUND_IF(!query_result.size(), "User not found");
    // THROW_NOT_FOUND_IF(!user_optional, "User not found");

    User found_user = query_result[0];
    crow::json::wvalue response_body{};
    response_body["public_key"] = found_user.get_public_key();

    return crow::response(200, response_body.dump());
}

crow::response key_update_handler(const crow::request& req, Database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    THROW_BAD_REQUEST_IF(!ValidateRequest(request, "public_key", "handle"),
                         "Invalid JSON format");

    auto users = db.get_collection<User>("users");
    auto query_result = db.get_collection<User>("users").filter_str_eq(
        {{"handle", request["handle"].s()}});
    THROW_NOT_FOUND_IF(!query_result.size(), "User not found");

    User old_user_data = query_result[0];
    User new_user_data = query_result[0];
    new_user_data.set_public_key(request["public_key"].s());

    users.update_one(old_user_data, new_user_data);

    return crow::response(200);
}
/*
// time format in json: %Y-%m-%d %H:%M:%S
crow::response message_send_handler(const crow::request& req,
                                    const mongocxx::database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    THROW_BAD_REQUEST_IF(
        !ValidateRequest(request, "sender", "reciever", "payload", "datetime",
                         "encrypted_by"),
        "Invalid JSON format");

    // checking users exist
    auto user_optional = find_user_by_handle(request["sender"].s(), db);
    THROW_NOT_FOUND_IF(!user_optional, "User not found");
    user_optional = find_user_by_handle(request["reciever"].s(), db);
    THROW_NOT_FOUND_IF(!user_optional, "User not found");
    user_optional = find_user_by_handle(request["encrypted_by"].s(), db);
    THROW_NOT_FOUND_IF(!user_optional, "User not found");

    std::istringstream time_sent(request["datetime"].s());
    std::tm parsed_time = {};
    time_sent >> std::get_time(&parsed_time, "%Y-%m-%d %H:%M:%S");

    THROW_BAD_REQUEST_IF(time_sent.fail(), "Failed to parse time string");

    std::time_t utc_time = std::mktime(&parsed_time);
    THROW_BAD_REQUEST_IF(-1 == utc_time, "Failed to build time from std::tm");

    bsoncxx::types::b_date bson_date{
        std::chrono::system_clock::from_time_t(utc_time)};

    bsoncxx::document::value new_message =
        make_document(kvp("sender", request["sender"].s()),
                      kvp("reciever", request["reciever"].s()),
                      kvp("payload", request["payload"].s()),
                      kvp("encrypted_by", request["encrypted_by"].s()),
                      kvp("datetime", bson_date));

    mongocxx::collection messages = db["messages"];

    messages.insert_one(std::move(new_message));

    return crow::response(200);
}*/
