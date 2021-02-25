#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
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
};

class Database {};

