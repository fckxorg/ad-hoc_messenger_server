#include <crow_all.h>

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

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

    CROW_ROUTE(app, "/")
    ([]() { return "Get the fuck out of my room, I'm playing minecraft!"; });

    CROW_ROUTE(app, "/user/find")
        .methods("POST"_method)([](const crow::request& req) {
            auto request = crow::json::load(req.body);

            if (!request || !request.has("handle")) {
                return crow::response(400);
            }

            std::cout << request["handle"] << std::endl;

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
                response_body["public_key"] = bsoncxx::to_json(*maybe_result);
            } else {
                response_body["status"] = "NULL";
            }

            crow::response response(response_body.dump());
            return response;
        });

    CROW_ROUTE(app, "/key/update")
        .methods("POST"_method)([](const crow::request& req) {
            auto request = crow::json::load(req.body);

            if (!request || !request.has("public_key")) {
                return crow::response(400);
            }

            return crow::response(200);
        });

    app.port(5000).multithreaded().run();
}

