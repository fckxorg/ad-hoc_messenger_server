#include <gtest/gtest.h>
#include <crow_all.h>
#include <cstring>

#include "request_helpers.hpp"

//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//              ValidateFold Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================

TEST(ValidateFold, TrueCase) {
    EXPECT_EQ(ValidateFold(true), true);
    EXPECT_EQ(ValidateFold(true, true), true);
    EXPECT_EQ(ValidateFold(true, true, true, true), true);
}

TEST(ValidateFold, FalseCase) {
    EXPECT_EQ(ValidateFold(false), false);
    EXPECT_EQ(ValidateFold(false, false), false);
    EXPECT_EQ(ValidateFold(false, false, false, false), false);
}

TEST(ValidateFold, TrueFalseCase) {
    EXPECT_EQ(ValidateFold(true, false), false);
    EXPECT_EQ(ValidateFold(true, true, false, true), false);
    EXPECT_EQ(ValidateFold(true, false, false, false), false);
}

//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//            ValidateRequest Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================

TEST(ValidateRequest, ValidCase) {
    const char* json = "{\"handle\":\"@fckxorg\", \"public_key\":\"OMG\" }";
    const int json_len = strlen(json);

    crow::json::rvalue json_rvalue = crow::json::load(json, json_len);

    EXPECT_EQ(ValidateRequest(json_rvalue), true);
    EXPECT_EQ(ValidateRequest(json_rvalue, "handle"), true);
    EXPECT_EQ(ValidateRequest(json_rvalue, "handle", "public_key"), true);
}

TEST(ValidateRequest, InvalidJSON) {
    const char* json = "{\"handle:\"@fckxorg\", \"public_key\":\"OMG\" }";
    const int json_len = strlen(json);

    crow::json::rvalue json_rvalue = crow::json::load(json, json_len);

    EXPECT_EQ(ValidateRequest(json_rvalue), false);
    EXPECT_EQ(ValidateRequest(json_rvalue, "handle"), false);
    EXPECT_EQ(ValidateRequest(json_rvalue, "handle", "public_key"), false);
}

TEST(ValidateRequest, NoRequiredField) {
    const char* json = "{\"handle\":\"@fckxorg\"}";
    const int json_len = strlen(json);

    crow::json::rvalue json_rvalue = crow::json::load(json, json_len);

    EXPECT_EQ(ValidateRequest(json_rvalue, "public_key"), false);
    EXPECT_EQ(ValidateRequest(json_rvalue, "handle", "public_key"), false);
}

TEST(ValidateRequest, NoRequiredFieldAndInvalidJSON) {
    const char* json = "{\"handle:\"@fckxorg\"}";
    const int json_len = strlen(json);

    crow::json::rvalue json_rvalue = crow::json::load(json, json_len);

    EXPECT_EQ(ValidateRequest(json_rvalue), false);
    EXPECT_EQ(ValidateRequest(json_rvalue, "public_key"), false);
    EXPECT_EQ(ValidateRequest(json_rvalue, "handle", "public_key"), false);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
