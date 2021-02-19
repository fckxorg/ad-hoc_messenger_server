#include <crow_all.h>
#include <stdarg.h>

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

template<typename... Args>
bool ValidateFold(Args... args) {
    return (args && ...);
}

template<typename... Args>
bool ValidateRequest(const crow::json::rvalue& request, Args... args) {
    return request && ValidateFold(request.has(args)...); 
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

            if (!maybe_result) {
                return crow::response(404);
            }

            crow::json::wvalue response_body;
            bsoncxx::document::value found_user = *maybe_result;

            response_body["public_key"] =
                bsoncxx::stdx::string_view(
                    found_user.view()["public_key"].get_utf8())
                    .data();

            return crow::response(200, response_body.dump());
        });

    CROW_ROUTE(app, "/key/update")
        .methods("POST"_method)([](const crow::request& req) {
            crow::json::rvalue request = crow::json::load(req.body);

            if (!ValidateRequest(request, "public_key") ||
                !ValidateRequest(request, "handle")) {
                return crow::response(400);
            }

            mongocxx::collection users = db["users"];

            auto builder = bsoncxx::builder::stream::document{};
            bsoncxx::document::value filter_document =
                builder << "handle" << request["handle"].s()
                        << bsoncxx::builder::stream::finalize;
            bsoncxx::document::value update_document =
                builder << "$set" << bsoncxx::builder::stream::open_document
                        << "public_key" << request["public_key"].s()
                        << bsoncxx::builder::stream::close_document
                        << bsoncxx::builder::stream::finalize;

            auto maybe_result = users.update_one(std::move(filter_document),
                                                 std::move(update_document));

            if (!maybe_result) {
                return crow::response(404);
            }

            return crow::response(200);
        });

    app.port(5000).multithreaded().run();
}

