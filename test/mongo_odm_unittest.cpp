#include "mongo_odm/mongo_odm.hpp"

#include <gtest/gtest.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
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
};

TEST(DBMappingClass, GetAndSet) {
    MappingMock mock_object{};

    mock_object.set("sample", 0);
    mock_object.set("other_sample", 1);

    EXPECT_EQ(mock_object.get(0) == "sample", true);
    EXPECT_EQ(mock_object.get(1) == "other_sample", true);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
