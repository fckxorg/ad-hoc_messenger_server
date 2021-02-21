#define CROW_MAIN_
#include <gtest/gtest.h>
#include <crow_all.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "handlers.hpp"

TEST(UserFindHandler, Success) {
    mongocxx::instance{};
    mongocxx::client client{mongocxx::uri{}};
    mongocxx::database test_db = client["messenger_db"];
    
    crow::request test_request{};
    test_request.body = "{\"handle\": \"@fckxorg\"}";
    test_request.method = "POST"_method;

    EXPECT_EQ(user_find_handler(test_request, test_db).code, crow::response(200).code);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
