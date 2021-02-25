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
    bsoncxx::document::value serialize() override { return make_document(); }
    void deserialize(const bsoncxx::document::value& data) override {}
};

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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
