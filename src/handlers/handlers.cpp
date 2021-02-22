#include "handlers.hpp"

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
