#include <crow_all.h>
#include <stdarg.h>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "request_helpers/request_helpers.hpp"

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

mongocxx::database db;

int main() {
    mongocxx::instance instance{};
    mongocxx::client client{mongocxx::uri{}};

    db = client["messenger_db"];

    crow::SimpleApp app;

    //========================================================================
    //
    //
    // ROUTES
    //
    //
    //========================================================================

    CROW_ROUTE(app, "/user/find")
        .methods("POST"_method)([](const crow::request& req) {
            crow::json::rvalue request = crow::json::load(req.body);

            if (!ValidateRequest(request, "handle")) {
                return crow::response(400);
            }

            bsoncxx::document::value filter =
                make_document(kvp("handle", request["handle"].s()));

            mongocxx::collection users = db["users"];
            auto maybe_result = users.find_one(std::move(filter));

            if (!maybe_result) {
                return crow::response(404);
            }

            bsoncxx::document::value found_user = *maybe_result;
            crow::json::wvalue response_body{};

            response_body["public_key"] =
                bsoncxx::stdx::string_view(
                    found_user.view()["public_key"].get_utf8())
                    .data();

            return crow::response(200, response_body.dump());
        });

    CROW_ROUTE(app, "/key/update")
        .methods("POST"_method)([](const crow::request& req) {
            crow::json::rvalue request = crow::json::load(req.body);

            if (!ValidateRequest(request, "public_key", "handle")) {
                return crow::response(400);
            }

            mongocxx::collection users = db["users"];

            bsoncxx::document::value filter =
                make_document(kvp("handle", request["handle"].s()));
            bsoncxx::document::value update = make_document(kvp(
                "$set",
                make_document(kvp("public_key", request["public_key"].s()))));

            auto maybe_result =
                users.update_one(std::move(filter), std::move(update));

            if (!maybe_result) {
                return crow::response(404);
            }

            return crow::response(200);
        });

    app.port(5000).multithreaded().run();
}

