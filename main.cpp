#include <crow_all.h>

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

mongocxx::database db;

bool ValidateRequest(const crow::json::rvalue& request,
                     const char* required_field) {
    return request && request.has(required_field);
}

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

            mongocxx::collection users = db["users"];

            auto filter_builder = bsoncxx::builder::stream::document{};
            bsoncxx::document::value filter_document =
                filter_builder << "handle" << request["handle"].s()
                               << bsoncxx::builder::stream::finalize;

            bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                users.find_one(std::move(filter_document));

            crow::json::wvalue response_body;
            if (maybe_result) {
                response_body["status"] = "OK";
                bsoncxx::document::value found_user = *maybe_result;
                response_body["payload"] = bsoncxx::stdx::string_view(found_user.view()["handle"].get_utf8()).data();
            } else {
                response_body["status"] = "NULL";
            }

            crow::response response(response_body.dump());
            return response;
        });

    CROW_ROUTE(app, "/key/update")
        .methods("POST"_method)([](const crow::request& req) {
            crow::json::rvalue request = crow::json::load(req.body);

            if (!ValidateRequest(request, "public_key")) {
                return crow::response(400);
            }

            return crow::response(200);
        });

    app.port(5000).multithreaded().run();
}

