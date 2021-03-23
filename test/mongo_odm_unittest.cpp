#include "mongo_odm.hpp"

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

#include "mongo_odm/mongo_odm.hpp"

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

  void deserialize(const bsoncxx::document::view& data) override {
    set(get_string_from_bson(data, "string"), 0);
    set(get_time_from_bson(data, "date"), 1);
  }
};

class ODMFixture : public ::testing::Test {
 protected:
  Database* db = new Database("test_messenger_db");
  MappingMock mock_object;

  virtual void SetUp() {
    mock_object.set("test", 0);
    mock_object.set(std::chrono::system_clock::now(), 1);
  }

  virtual void TearDown() {
    auto col = db->get_collection<MappingMock>("mock");
    auto entries = col.apply();

    for (auto& el : entries) {
      col.delete_one(el);
    }
  }
};

TEST(DBMappingClass, GetAndSetString) {
  MappingMock mock_object{};

  std::string sample{"test_string"};

  mock_object.set(sample, 0);
  EXPECT_EQ(static_cast<std::string>(std::get<0>(mock_object.get(0))), sample);
}

TEST(DBMappingClass, GetAndSetTime) {
  MappingMock mock_object{};

  std::chrono::system_clock::time_point sample =
      std::chrono::system_clock::now();

  mock_object.set(sample, 0);

  auto result = std::get<1>(mock_object.get(0));

  EXPECT_EQ(result, sample);
}

TEST_F(ODMFixture, get_collection_v1) {
  auto col = db->get_collection<MappingMock>("mock");
}

TEST_F(ODMFixture, get_Collection_v2) {
  auto col = db->get_collection<MappingMock>(std::string("mock"));
}

TEST_F(ODMFixture, insert_one) {
  auto col = db->get_collection<MappingMock>("mock");
  col.insert_one(mock_object);
}

TEST_F(ODMFixture, test_apply) {
  auto col = db->get_collection<MappingMock>("mock");
  col.insert_one(mock_object);
  auto result = col.apply();
  ASSERT_EQ(result.size(), 1);
}

TEST_F(ODMFixture, test_delete) {
  auto col = db->get_collection<MappingMock>("mock");
  col.insert_one(mock_object);
  auto result = col.apply();

  ASSERT_EQ(result.size(), 1);

  col.delete_one(mock_object);
  result = col.apply();

  ASSERT_EQ(result.size(), 0);
}

TEST_F(ODMFixture, test_filter_str_equal) {
  auto col = db->get_collection<MappingMock>("mock");
  col.insert_one(mock_object);

  MappingMock new_mock;
  new_mock.set("alter", 0);
  new_mock.set(std::chrono::system_clock::now(), 1);
  col.insert_one(new_mock);

  auto result = col.filter({{"string", "alter"}}).apply();

  ASSERT_EQ(result.size(), 1);

  result = col.filter({{"string", "alt"}}).apply();

  ASSERT_EQ(result.size(), 0);
}

TEST_F(ODMFixture, test_update) {
  auto col = db->get_collection<MappingMock>("mock");
  col.insert_one(mock_object);

  MappingMock new_mock = mock_object;
  new_mock.set("alter", 0);

  col.update_one(mock_object, new_mock);

  auto result = col.filter({{"string", "alter"}}).apply();
  ASSERT_EQ(result.size(), 1);

  result = col.filter({{"string", "test"}}).apply();
  ASSERT_EQ(result.size(), 0);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
