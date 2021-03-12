#include "mongo_odm/mongo_odm.hpp"

#include <gtest/gtest.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <chrono>
#include <cstdint>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

class MappingMock : public DBMapping<2> {
   public:
    bsoncxx::document::value serialize() override {
        return make_document(
            kvp("string", std::get<0>(get(0))),
            kvp("date", bsoncxx::types::b_date(std::get<1>(get(1)))));
    }
    void deserialize(const bsoncxx::document::view& data) override {}
};

Database* db = new Database("test_messenger_db");

TEST(DBMappingClass, GetAndSetString) {
    MappingMock mock_object{};

    std::string sample{"test_string"};

    mock_object.set(sample, 0);
    EXPECT_EQ(static_cast<std::string>(std::get<0>(mock_object.get(0))),
              sample);
}

TEST(DBMappingClass, GetAndSetTime) {
    MappingMock mock_object{};

    std::chrono::system_clock::time_point sample =
        std::chrono::system_clock::now();

    mock_object.set(sample, 0);

    auto result = std::get<1>(mock_object.get(0));

    EXPECT_EQ(result, sample);
}

TEST(DatabaseClass, get_collection_v1) {
    auto col = db->get_collection<MappingMock>("mock");
}

TEST(DatabaseClass, get_Collection_v2) {
    auto col = db->get_collection<MappingMock>(std::string("mock"));
}

TEST(CollectionClass, insert_one) {
    MappingMock mock_object{};
    mock_object.set("test", 0);
    mock_object.set(std::chrono::system_clock::now(), 1);

    auto col = db->get_collection<MappingMock>("mock");
    col.insert_one(mock_object);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
