#include <iostream>
#include <crow_all.h>

#include <mongocxx/instance.hpp>

int main() {
    mongocxx::instance instance{};

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([]() { return "Get the fuck out of my room, I'm playing minecraft!"; });

    CROW_ROUTE(app, "/user/find")
        .methods("POST"_method)([](const crow::request& req) {
            auto request = crow::json::load(req.body);

            if (!request || !request.has("handle")) {
                return crow::response(400);
            }

            std::cout << request["handle"] << std::endl;

            crow::json::wvalue response_body;
            response_body["status"] = "OK";
            response_body["public_key"] = "key";

            crow::response response(response_body.dump());
            return response;
        });

    CROW_ROUTE(app, "/key/update")
        .methods("POST"_method)([](const crow::request& req) {
            auto request = crow::json::load(req.body);

            if (!request || !request.has("public_key")) {
                return crow::response(400);
            }
        });

    app.port(5000).multithreaded().run();
}

