#include <chrono>
#define CROW_MAIN_
#include "request_helpers.hpp"

#include <crow_all.h>
#include <gtest/gtest.h>

#include <cstring>

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

TEST(str_to_tp, Success) {
    const std::string timestamp = "2021-02-20 23:42:15";
    std::tm time = {.tm_sec = 15,
                    .tm_min = 42,
                    .tm_hour = 23,
                    .tm_mday = 20,
                    .tm_mon = 1,
                    .tm_year = 121};

    auto desired_tp = std::chrono::system_clock::from_time_t(std::mktime(&time));

    ASSERT_EQ(desired_tp, *str_to_tp(timestamp));
}

TEST(str_to_tp, Failure) {
    const std::string timestamp = "2021-02-20-23-42-15"; 

    ASSERT_EQ(str_to_tp(timestamp).has_value(), false);
}

TEST(tp_to_str, Success) {
    const std::string timestamp = "2020-02-20 23:42:15";
    ASSERT_EQ(tp_to_str(*str_to_tp(timestamp)), timestamp);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
