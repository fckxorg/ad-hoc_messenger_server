#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <string>

class Collection {
   private:
    mongocxx::collection col;
   public:
    
    Collection(const char* collectionName, mongocxx::client& client);

};

class DB {
   private:
    static mongocxx::instance instance;
    static mongocxx::client client;

   public:
    DB() = delete;

    static bool init(const char* dbName);
    static bool init(const std::string& dbName);

    static Collection GetCollection(const char* collectionName);
    static Collection GetCollection(const std::string& collectionName);
};
