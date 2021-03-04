#define CROW_MAIN_

#include "handlers.hpp"

#include <crow_all.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "models.hpp"
#include "mongo_odm.hpp"

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// shared mongo instance
mongocxx::instance instance{};

class HandlerTestFixture : public ::testing::Test {
   private:
    User fckxorg;
    User coffee;

   protected:
    Database* db;

    virtual void SetUp() {
        db = new Database("messenger_test_db");

        fckxorg.set_email("max.kokryashkiN@gmail.com");
        fckxorg.set_handle("@fckxorg");
        fckxorg.set_public_key("fckxorg_key");

        coffee.set_email("test@test.com");
        coffee.set_handle("@COFF33");
        coffee.set_public_key("coffee_key");

        auto users = db->get_collection<User>("users");
        users.insert_one(fckxorg);
        users.insert_one(coffee);
    }

    virtual void TearDown() {
        auto users = db->get_collection<User>("users");
        users.delete_one(fckxorg);
        users.delete_one(coffee);

        delete db;
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

    crow::response test_response = user_find_handler(test_request, *db);
    crow::json::rvalue response_body = crow::json::load(test_response.body);

    EXPECT_EQ(test_response.code, 200);
    EXPECT_EQ(response_body.has("public_key"), true);
}

TEST_F(HandlerTestFixture, UserFindHandler_InvalidJson) {
    crow::request test_request{};
    test_request.body = "{\"handle: \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_find_handler(test_request, *db).code, 400);
}

TEST_F(HandlerTestFixture, UserFindHandler_UserNotFound) {
    crow::request test_request{};
    test_request.body = "{\"handle\": \"@C0FF33\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_find_handler(test_request, *db).code, 404);
}

//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//              KeyUpdateHandler Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================

TEST_F(HandlerTestFixture, KeyUpdateHandler_Success) {
    crow::request test_request{};
    test_request.body =
        "{\"handle\": \"@fckxorg\", \"public_key\": \"new_key\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(key_update_handler(test_request, *db).code, 200);
}

TEST_F(HandlerTestFixture, KeyUpdateHandle_InvalidJSON) {
    crow::request test_request{};
    test_request.body =
        "{\"handle\": \"@fckxorg\", \"pubic_key\": \"new_key\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(key_update_handler(test_request, *db).code, 400);
}

TEST_F(HandlerTestFixture, KeyUpdateHandler_UserNotFound) {
    crow::request test_request{};
    test_request.body =
        "{\"handle\": \"@fckxor\", \"public_key\": \"new_key\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(key_update_handler(test_request, *db).code, 404);
}

//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//              MessageSendHandler Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================
/*
TEST_F(HandlerTestFixture, MessageSendHandler_Success) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxorg\", \"reciever\" : \"@COFF33\", \"payload\" : "
        "\"Are you winning, son?\", \"datetime\" : \"2021-02-24 00:07:00\", "
        "\"encrypted_by\" : \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *test_db).code, 200);
}

TEST_F(HandlerTestFixture, MessageSendHandler_InvalidJSON) {
    crow::request test_request{};
    test_request.body =
        "{\"reciever\" : \"@COFF33\", payload\" : \"Are you winning, son?\", "
        "\"datetime\" : \"2021-02-24 00:07:00\", \"encrypted_by\" : "
        "\"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *test_db).code, 400);
}

TEST_F(HandlerTestFixture, MessageSendHandler_InvalidTimeFormat) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxorg\", \"reciever\" : \"@COFF33\", \"payload\" : "
        "\"Are you winning, son?\", \"datetime\" : \"21:02-24 00:07-00\", "
        "\"encrypted_by\" : \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *test_db).code, 400);
}

TEST_F(HandlerTestFixture, MessageSendHandler_SenderDoesntExist) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxor\", \"reciever\" : \"@COFF33\", \"payload\" : "
        "\"Are you winning, son?\", \"datetime\" : \"2021-02-24 00:07:00\", "
        "\"encrypted_by\" : \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *test_db).code, 404);
}

TEST_F(HandlerTestFixture, MessageSendHandler_RecieverDoesntExist) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxorg\", \"reciever\" : \"@COFF3\",\"payload\" : "
        "\"Are you winning, son?\",\"datetime\" : \"2021-02-24 00:07:00\", "
        "\"encrypted_by\" : \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *test_db).code, 404);
}

TEST_F(HandlerTestFixture, MessageSendHandler_EncryptorDoesntExist) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxorg\", \"reciever\" : \"@COFF33\", \"payload\" : "
        "\"Are you winning, son?\", \"datetime\" : \"2021-02-24 00:07:00\", "
        "\"encrypted_by\" : \"@fckxor\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *test_db).code, 404);
}
*/
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
