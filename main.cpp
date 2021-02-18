#include <crow_all.h>

#include <iostream>

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() { return "Get the fuck out of my room, I'm playing minecraft!"; });

    CROW_ROUTE(app, "/user/find")
        .methods("POST"_method)([](const crow::request& req) {
            auto request = crow::json::load(req.body);

            if(!request || !request.has("handle")) {
                return crow::response(400);
            }
            
            std::cout << request["handle"] << std::endl;


            crow::json::wvalue response_body;
            response_body["status"] = "OK";
            response_body["public_key"] = "key";
            
            crow::response response(response_body.dump());
            return response;
        });
    
    CROW_ROUTE(app, "/user/auth")
        .methods("GET"_method, "POST"_method)([](const crow::request& req) {
            auto request = crow::json::load(req.body);    
            return "OK";
        });

    app.port(5000).multithreaded().run();
}

