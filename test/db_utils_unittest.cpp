#define CROW_MAIN_
#include "db_utils.hpp"

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
#include <string>

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

mongocxx::instance instance{};

class DBUtilitiesTestFixture : public ::testing::Test {
   protected:
    mongocxx::client client;
    mongocxx::database* test_db;

    virtual void SetUp() {
        client = mongocxx::client(mongocxx::uri{});
        test_db = new mongocxx::database(client["test_messenger_db"]);

        bsoncxx::document::value test_user_fckxorg = make_document(
            kvp("handle", "@fckxorg"), kvp("public_key", "my_key"));

        auto users = (*test_db)["users"];
        users.insert_one(std::move(test_user_fckxorg));
    }

    virtual void TearDown() {
        bsoncxx::document::value test_user =
            make_document(kvp("handle", "@fckxorg"));
        auto users = (*test_db)["users"];
        users.delete_one(std::move(test_user));

        delete test_db;
    }
};

//=================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//              FindUserByHandle Tests
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//=================================================

TEST_F(DBUtilitiesTestFixture, FindUserByHandle_Success) {
    EXPECT_EQ(static_cast<bool>(find_user_by_handle("@fckxorg", *test_db)),
              true);
}

TEST_F(DBUtilitiesTestFixture, FindUserByHandle_Failure) {
    EXPECT_EQ(static_cast<bool>(find_user_by_handle("@fckxor", *test_db)),
              false);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
