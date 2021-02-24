#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <numeric>
#include <string>
#include <string_view>

template <uint32_t n_properties>
class DBMapping {
   private:
    std::string properties[n_properties];

   public:
    std::string_view get(uint32_t property_id) {
        assert(property_id < n_properties);

        return properties[property_id];
    }

    void set(const char* data, uint32_t property_id) {
        assert(property_id < n_properties);
        assert(data != nullptr);
        properties[property_id] = data;
    }

    virtual bsoncxx::document::value serialize() = 0;
};

