#define CROW_MAIN_ // withouth this define multiple definiton error os likely to occur
#include <crow_all.h>
#include <stdarg.h>

#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "handlers/handlers.hpp"

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
            return user_find_handler(req, db);
        });

    CROW_ROUTE(app, "/key/update")
        .methods("POST"_method)([](const crow::request& req) {
            return key_update_handler(req, db);
        });

    app.port(5000).multithreaded().run();
}

