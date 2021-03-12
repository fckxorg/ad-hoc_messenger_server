#include "handlers.hpp"

#include "request_helpers.hpp"

crow::response user_find_handler(const crow::request& req, Database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    THROW_BAD_REQUEST_IF(!ValidateRequest(request, "handle"),
                         "Invalid JSON format");

    auto query_result = db.get_collection<User>("users")
                            .filter_str_eq({{"handle", request["handle"].s()}})
                            .apply();
    THROW_NOT_FOUND_IF(!query_result.size(), "User not found");

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
    auto query_result = db.get_collection<User>("users")
                            .filter_str_eq({{"handle", request["handle"].s()}})
                            .apply();
    THROW_NOT_FOUND_IF(!query_result.size(), "User not found");

    User old_user_data = query_result[0];
    User new_user_data = query_result[0];
    new_user_data.set_public_key(request["public_key"].s());

    users.update_one(old_user_data, new_user_data);

    return crow::response(200);
}

crow::response message_send_handler(const crow::request& req, Database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    THROW_BAD_REQUEST_IF(
        !ValidateRequest(request, "sender", "reciever", "payload", "datetime",
                         "encrypted_by"),
        "Invalid JSON format");

    auto messages = db.get_collection<Message>("messages");

    bool not_found = !User::exists(request["sender"].s(), db) ||
                     !User::exists(request["reciever"].s(), db) ||
                     !User::exists(request["encrypted_by"].s(), db);

    THROW_NOT_FOUND_IF(not_found, "User not found");

    auto tp = str_to_tp(request["datetime"].s());
    THROW_BAD_REQUEST_IF(!tp, "Invalid datetime format");

    Message new_message{};
    new_message.set_sender(request["sender"].s());
    new_message.set_reciever(request["reciever"].s());
    new_message.set_payload(request["payload"].s());
    new_message.set_encrypted_by(request["encrypted_by"].s());
    new_message.set_datetime(*tp);

    messages.insert_one(new_message);

    return crow::response(200);
}

crow::response message_get_handler(const crow::request& req, Database& db) {
    crow::json::rvalue request = crow::json::load(req.body);

    THROW_BAD_REQUEST_IF(
        !ValidateRequest(request, "encryptor", "partner", "from", "to"),
        "Invalid JSON");

    bool not_found = !User::exists(request["encryptor"].s(), db) ||
                     !User::exists(request["partner"].s(), db);

    THROW_NOT_FOUND_IF(not_found, "User not found");

    auto from_tp = str_to_tp(request["from"].s());
    auto to_tp = str_to_tp(request["to"].s());

    THROW_BAD_REQUEST_IF(!from_tp, "Invalid time format");
    THROW_BAD_REQUEST_IF(!to_tp, "Invalid time format");
    THROW_BAD_REQUEST_IF(*to_tp <= *from_tp, "Bad time boundaries");

    auto messages = db.get_collection<Message>("messages");

    auto messages_query_sender =
        messages.filter_time_between(*from_tp, *to_tp)
            .filter_str_eq({{"sender", request["encryptor"].s()},
                            {"reciever", request["partner"].s()},
                            {"encrypted_by", request["encryptor"].s()}})
            .apply();

    auto messages_query_reciever =
        messages.filter_time_between(*from_tp, *to_tp)
            .filter_str_eq({{"sender", request["partner"].s()},
                            {"reciever", request["encryptor"].s()},
                            {"encrypted_by", request["encryptor"].s()}})
            .apply();

    std::vector<crow::json::wvalue> serialized_messages = {};
    for (auto& message : messages_query_sender) {
        crow::json::wvalue json_message = {};
        json_message["sender"] = message.get_sender();
        json_message["reciever"] = message.get_reciever();
        json_message["encrypted_by"] = message.get_encrypted_by();
        json_message["payload"] = message.get_payload();
        json_message["datetime"] = tp_to_str(message.get_datetime());

        serialized_messages.push_back(std::move(json_message));
    }

    for (auto& message : messages_query_reciever) {
        crow::json::wvalue json_message = {};
        json_message["sender"] = message.get_sender();
        json_message["reciever"] = message.get_reciever();
        json_message["encrypted_by"] = message.get_encrypted_by();
        json_message["payload"] = message.get_payload();
        json_message["datetime"] = tp_to_str(message.get_datetime());

        serialized_messages.push_back(std::move(json_message));
    }

    crow::json::wvalue response_body = {};
    response_body["messages"] = std::move(serialized_messages);

    return crow::response(200, response_body.dump());
}
