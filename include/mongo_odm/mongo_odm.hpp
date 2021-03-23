#ifndef MONGO_ODM_HPP
#define MONGO_ODM_HPP

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <numeric>
#include <string>
#include <string_view>
#include <variant>

template <uint32_t n_properties>
class DBMapping {
 private:
  using mapping_variant =
      std::variant<std::string, std::chrono::system_clock::time_point>;
  mapping_variant properties[n_properties];

 protected:
  // TODO error-checking
  std::string get_string_from_bson(const bsoncxx::document::view& doc,
                                   const std::string& property) {
    return std::string(
        bsoncxx::stdx::string_view(doc[property].get_utf8()).data());
  }

  std::chrono::system_clock::time_point get_time_from_bson(
      const bsoncxx::document::view& doc, const std::string& property) {
    return static_cast<std::chrono::system_clock::time_point>(
        doc[property].get_date());
  }

 public:
  enum TYPES { STRING = 0, TIME = 1 };

  mapping_variant get(uint32_t property_id) {
    assert(property_id < n_properties);

    return properties[property_id];
  }

  void set(const std::string& data, uint32_t property_id) {
    assert(property_id < n_properties);

    properties[property_id] = data;
  }

  void set(const std::chrono::system_clock::time_point& data,
           uint32_t property_id) {
    properties[property_id] = data;
  }

  virtual bsoncxx::document::value serialize() = 0;
  virtual void deserialize(const bsoncxx::document::view& data) = 0;
};

template <typename MappingType>
class Collection {
 private:
  mongocxx::collection col;
  bsoncxx::builder::basic::document filter_doc;

 public:
  Collection() = delete;
  ~Collection() = default;

  Collection(const mongocxx::collection& other) { col = other; }

  Collection<MappingType>& filter(
      std::unordered_map<std::string, std::string> query) {
    for (auto& pair : query) {
      filter_doc.append(bsoncxx::builder::basic::kvp(pair.first, pair.second));
    }

    return *this;
  }

  Collection<MappingType>& filter(
      const std::chrono::system_clock::time_point& from,
      const std::chrono::system_clock::time_point& to) {
    // converting time to bson time format
    bsoncxx::types::b_date bson_from{from};
    bsoncxx::types::b_date bson_to{to};

    filter_doc.append(bsoncxx::builder::basic::kvp(
        "datetime",
        [bson_from, bson_to](bsoncxx::builder::basic::sub_document subdoc) {
          subdoc.append(bsoncxx::builder::basic::kvp("$gt", bson_from),
                        bsoncxx::builder::basic::kvp("$lte", bson_to));
        }));

    return *this;
  }

  std::vector<MappingType> apply() {
    mongocxx::cursor cursor = col.find(filter_doc.extract());

    std::vector<MappingType> result{};

    for (auto& doc : cursor) {
      MappingType mapping{};
      mapping.deserialize(doc);
      result.push_back(mapping);
    }

    filter_doc = {};
    return result;
  }

  void insert_one(MappingType& doc) {
    auto bson_doc = doc.serialize();
    col.insert_one(std::move(bson_doc));
  }

  void delete_one(MappingType& doc) {
    auto bson_doc = doc.serialize();
    col.delete_one(std::move(bson_doc));
  }

  void update_one(MappingType& old_doc, MappingType& new_doc) {
    delete_one(old_doc);
    insert_one(new_doc);
  }
};

class Database {
 private:
  static mongocxx::instance instance;
  mongocxx::client client;
  mongocxx::database db;

 public:
  Database(const std::string& name) : client{mongocxx::uri()} {
    db = client[name];
  }

  template <typename MappingType>
  Collection<MappingType> get_collection(const std::string& collection_name) {
    return Collection<MappingType>(db[collection_name]);
  }

  template <typename MappingType>
  Collection<MappingType> get_collection(const char* collection_name) {
    return Collection<MappingType>(db[collection_name]);
  }
};

#endif
