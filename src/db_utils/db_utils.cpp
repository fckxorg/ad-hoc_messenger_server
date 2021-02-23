#include "db_utils.hpp"

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

mongocxx::stdx::optional<bsoncxx::document::value> find_user_by_handle(
    const std::string& handle, const mongocxx::database& db) {
    mongocxx::collection users = db["users"];
    bsoncxx::document::value filter_document =
        make_document(kvp("handle", handle));
    return users.find_one(std::move(filter_document));
}
