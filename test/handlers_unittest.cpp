#define CROW_MAIN_
#include <gtest/gtest.h>
#include <crow_all.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "handlers.hpp"

mongocxx::instance instance{}; // intializing database
//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//              UserFindHandler Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================
//
TEST(UserFindHandler, Success) {
    mongocxx::client client{mongocxx::uri{}};
    mongocxx::database test_db = client["messenger_db"];
    
    crow::request test_request{};
    test_request.body = "{\"handle\": \"@fckxorg\"}";
    test_request.method = "POST"_method;


    crow::response test_response = user_find_handler(test_request, test_db);
    crow::json::rvalue response_body = crow::json::load(test_response.body);


    EXPECT_EQ(test_response.code, 200);
    EXPECT_EQ(response_body.has("public_key"), true); 
}

TEST(UserFindHandler, InvalidJson) {
    
    mongocxx::client client{mongocxx::uri{}};
    mongocxx::database test_db = client["messenger_db"];
    
    crow::request test_request{};
    test_request.body = "{\"handle: \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_find_handler(test_request, test_db).code, 400);
}

TEST(UserFindHandler, UserNotFound) {
    
    mongocxx::client client{mongocxx::uri{}};
    mongocxx::database test_db = client["messenger_db"];
    
    crow::request test_request{};
    test_request.body = "{\"handle\": \"@C0FF33\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_find_handler(test_request, test_db).code, 404);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
