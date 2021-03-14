#include "request_helpers.hpp"
#define CROW_MAIN_

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

#include "handlers.hpp"
#include "models.hpp"
#include "mongo_odm.hpp"

class HandlerTestFixture : public ::testing::Test {
   private:
    User fckxorg;
    User coffee;
    Message fckxorg_message;
    Message coffee_message;

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

        fckxorg_message =
            Message("@fckxorg", "@COFF33", "Ayyy LMAO", "@fckxorg",
                    *str_to_tp("2021-02-20 21:40:32"));

        coffee_message = Message("@COFF33", "@fckxorg", "Well, fuck...",
                                 "@fckxorg", *str_to_tp("2021-02-20 21:41:25"));

        auto users = db->get_collection<User>("users");
        users.insert_one(fckxorg);
        users.insert_one(coffee);

        auto messages = db->get_collection<Message>("messages");
        messages.insert_one(fckxorg_message);
        messages.insert_one(coffee_message);
    }

    virtual void TearDown() {
        auto users = db->get_collection<User>("users");
        users.delete_one(fckxorg);
        users.delete_one(coffee);

        auto messages = db->get_collection<Message>("messages");
        messages.delete_one(fckxorg_message);
        messages.delete_one(coffee_message);

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

TEST_F(HandlerTestFixture, MessageSendHandler_Success) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxorg\", \"reciever\" : \"@COFF33\", \"payload\" : "
        "\"Are you winning, son?\", \"datetime\" : \"2021-02-24 00:07:00\", "
        "\"encrypted_by\" : \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *db).code, 200);
}

TEST_F(HandlerTestFixture, MessageSendHandler_InvalidJSON) {
    crow::request test_request{};
    test_request.body =
        "{\"reciever\" : \"@COFF33\", payload\" : \"Are you winning, son?\", "
        "\"datetime\" : \"2021-02-24 00:07:00\", \"encrypted_by\" : "
        "\"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *db).code, 400);
}

TEST_F(HandlerTestFixture, MessageSendHandler_InvalidTimeFormat) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxorg\", \"reciever\" : \"@COFF33\", \"payload\" : "
        "\"Are you winning, son?\", \"datetime\" : \"21:02-24 00:07-00\", "
        "\"encrypted_by\" : \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *db).code, 400);
}

TEST_F(HandlerTestFixture, MessageSendHandler_SenderDoesntExist) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxor\", \"reciever\" : \"@COFF33\", \"payload\" : "
        "\"Are you winning, son?\", \"datetime\" : \"2021-02-24 00:07:00\", "
        "\"encrypted_by\" : \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *db).code, 404);
}

TEST_F(HandlerTestFixture, MessageSendHandler_RecieverDoesntExist) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxorg\", \"reciever\" : \"@COFF3\",\"payload\" : "
        "\"Are you winning, son?\",\"datetime\" : \"2021-02-24 00:07:00\", "
        "\"encrypted_by\" : \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *db).code, 404);
}

TEST_F(HandlerTestFixture, MessageSendHandler_EncryptorDoesntExist) {
    crow::request test_request{};
    test_request.body =
        "{\"sender\" : \"@fckxorg\", \"reciever\" : \"@COFF33\", \"payload\" : "
        "\"Are you winning, son?\", \"datetime\" : \"2021-02-24 00:07:00\", "
        "\"encrypted_by\" : \"@fckxor\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_send_handler(test_request, *db).code, 404);
}

//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//              MessageGetHandler Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================

TEST_F(HandlerTestFixture, MessageGetHandler_Success) {
    crow::request test_request{};
    test_request.body =
        "{\"from\" : \"2021-01-02 21:00:00\", \"to\" : \"2021-04-23 21:00:00\","
        "\"encryptor\" : \"@fckxorg\","
        "\"partner\" : \"@COFF33\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_get_handler(test_request, *db).code, 200);
}

TEST_F(HandlerTestFixture, MessageGetHandler_InvalidJson) {
    crow::request test_request{};
    test_request.body =
        "{\"from\" : \"2021-01-02 21:00:00\", \"to\" : \"2021-04-23 21:00:00\","
        "\"partner\" : \"@COFF33\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_get_handler(test_request, *db).code, 400);
}

TEST_F(HandlerTestFixture, MessageGetHandler_UserDoesntExist) {
    crow::request test_request{};
    test_request.body =
        "{\"from\" : \"2021-01-02 21:00:00\", \"to\" : \"2021-04-23 21:00:00\","
        "\"encryptor\" : \"@fkxorg\","
        "\"partner\" : \"@COF33\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_get_handler(test_request, *db).code, 404);
}

TEST_F(HandlerTestFixture, MessageGetHandler_InvalidTimeFormatFrom) {
    crow::request test_request{};
    test_request.body =
        "{\"from\" : \"2021-01-02-21:00:00\", \"to\" : \"2021-04-23 21:00:00\","
        "\"encryptor\" : \"@fckxorg\","
        "\"partner\" : \"@COFF33\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_get_handler(test_request, *db).code, 400);
}

TEST_F(HandlerTestFixture, MessageGetHandler_InvalidTimeFormatTo) {
    crow::request test_request{};
    test_request.body =
        "{\"from\" : \"2021-01-02 21:00:00\", \"to\" : \"2021-04-23-21:00:00\","
        "\"encryptor\" : \"@fckxorg\","
        "\"partner\" : \"@COFF33\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_get_handler(test_request, *db).code, 400);
}

TEST_F(HandlerTestFixture, MessageGetHandler_BadTimeBounds) {
    crow::request test_request{};
    test_request.body =
        "{\"from\" : \"2021-04-23 21:00:00\", \"to\" : \"2021-01-02 21:00:00\","
        "\"encryptor\" : \"@fckxorg\","
        "\"partner\" : \"@COFF33\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(message_get_handler(test_request, *db).code, 400);
}

//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//              UserRegisterHandler Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================

TEST_F(HandlerTestFixture, UserRegisterHandler_Success) {
    crow::request test_request{};
    test_request.body =
        "{\"handle\":\"@boristab\", \"email\":\"test@test.com\", "
        "\"public_key\":\"omg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_register_handler(test_request, *db).code, 200);

    auto users = db->get_collection<User>("users");
    EXPECT_TRUE(User::exists("@boristab", *db));

    User boristab{};
    boristab.set_handle("@boristab");
    boristab.set_email("test@test.com");
    boristab.set_public_key("omg");

    users.delete_one(boristab);
}

TEST_F(HandlerTestFixture, UserRegisterHandler_InvalidJson) {
    crow::request test_request{};
    test_request.body =
        "{\"handle\":\"@boristab\", \"eail\":\"test@test.com\", "
        "\"public_key\":\"omg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_register_handler(test_request, *db).code, 400);
}

TEST_F(HandlerTestFixture, UserRegisterHandler_UserExists) {
    crow::request test_request{};
    test_request.body =
        "{\"handle\":\"@fckxorg\", \"email\":\"test@test.com\", "
        "\"public_key\":\"omg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_register_handler(test_request, *db).code, 400);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
