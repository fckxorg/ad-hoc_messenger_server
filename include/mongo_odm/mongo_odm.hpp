#ifndef MONGO_ODM_HPP
#define MONGO_ODM_HPP

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
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

   public:
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
    virtual void deserialize(const bsoncxx::document::value& data) = 0;
};

template <typename MappingType>
class Collection {
   private:
    mongocxx::collection col;

   public:
    Collection() = delete;
    ~Collection() = default;

    Collection(const mongocxx::collection& other) { col = other; }

    std::vector<MappingType> filter_str_eq(
        std::unordered_map<std::string, std::string> query) {
        bsoncxx::builder::basic::document query_builder{};

        for (auto& pair : query) {
            query_builder.append(
                bsoncxx::builder::basic::kvp(pair.first, pair.second));
        }

        mongocxx::cursor cursor = col.find(query_builder.extract());

        std::vector<MappingType> result{};

        for (auto& doc : cursor) {
            MappingType mapping{};
            mapping.deserialize(doc);
            result.push_back(mapping);
        }

        return result;
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
    Collection<MappingType> operator[](const std::string& collection_name) {
        return Collection<MappingType>(db[collection_name]);
    }
};

#endif
