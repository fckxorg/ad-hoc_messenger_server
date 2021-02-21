#define CROW_MAIN_
#include <gtest/gtest.h>
#include <crow_all.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include "handlers.hpp"

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// shared mongo instance
mongocxx::instance instance{};

class HandlerTestFixture : public ::testing::Test {
    protected:
        mongocxx::client client;
        mongocxx::database* test_db;

        virtual void SetUp() {
           client = mongocxx::client(mongocxx::uri{});
           test_db = new mongocxx::database(client["test_messenger_db"]);

           bsoncxx::document::value test_user = make_document(kvp("handle", "@fckxorg"), kvp("public_key", "my_key"));

           auto users = (*test_db)["users"];
           users.insert_one(std::move(test_user));
        }

        virtual void TearDown() {
           bsoncxx::document::value test_user = make_document(kvp("handle", "@fckxorg"));
           auto users = (*test_db)["users"];
           users.delete_one(std::move(test_user));

           delete test_db;
        }
};

//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//              UserFindHandler Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================

TEST_F(HandlerTestFixture, UserFindHandler_Success) {
    crow::request test_request{};
    test_request.body = "{\"handle\": \"@fckxorg\"}";
    test_request.method = "POST"_method;


    crow::response test_response = user_find_handler(test_request, *test_db);
    crow::json::rvalue response_body = crow::json::load(test_response.body);


    EXPECT_EQ(test_response.code, 200);
    EXPECT_EQ(response_body.has("public_key"), true); 
}

TEST_F(HandlerTestFixture, UserFindHandler_InvalidJson) {
    crow::request test_request{};
    test_request.body = "{\"handle: \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_find_handler(test_request, *test_db).code, 400);
}

TEST_F(HandlerTestFixture, UserFindHandler_UserNotFound) {
    crow::request test_request{};
    test_request.body = "{\"handle\": \"@C0FF33\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_find_handler(test_request, *test_db).code, 404);
}

//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//              KeyUpdateHandler Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================

TEST_F(HandlerTestFixture, KeyUpdateHandler_Success) {
    crow::request test_request{};
    test_request.body = "{\"handle\": \"@fckxorg\", \"public_key\": \"new_key\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(key_update_handler(test_request, *test_db).code, 200);
}

TEST_F(HandlerTestFixture, KeyUpdateHandle_InvalidJSON) {
    crow::request test_request{};
    test_request.body = "{\"handle\": \"@fckxorg\", \"pubic_key\": \"new_key\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(key_update_handler(test_request, *test_db).code, 400);
}

TEST_F(HandlerTestFixture, KeyUpdateHandler_UserNotFound) {
    crow::request test_request{};
    test_request.body = "{\"handle\": \"@COFF33\", \"public_key\": \"new_key\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(key_update_handler(test_request, *test_db).code, 404);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
