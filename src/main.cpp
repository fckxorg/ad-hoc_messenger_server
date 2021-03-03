#define CROW_MAIN_  // withouth this define multiple definiton error os likely
                    // to occur
#include <crow_all.h>
#include <stdarg.h>

#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "handlers/handlers.hpp"
#include "mongo_odm/mongo_odm.hpp"

int main() {
    crow::SimpleApp app;
    Database db{"messenger_db"};

    //========================================================================
    //
    //
    // ROUTES
    //
    //
    //========================================================================

    CROW_ROUTE(app, "/user/find")
        .methods("POST"_method)([&db](const crow::request& req) {
            return user_find_handler(req, db);
        });

    CROW_ROUTE(app, "/key/update")
        .methods("POST"_method)([&db](const crow::request& req) {
            return key_update_handler(req, db);
        });
    CROW_ROUTE(app, "/message/send")
        .methods("POST"_method)([&db](const crow::request& req) {
            return message_send_handler(req, db);
        });

    app.port(5000).multithreaded().run();
}

