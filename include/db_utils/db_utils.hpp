#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <string>

mongocxx::stdx::optional<bsoncxx::document::value> find_user_by_handle(const std::string& handle, const mongocxx::database& db);
