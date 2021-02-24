#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <string>

/*
 * TODO place this classes into src/models
class User : DBMapping<3> {
   public:
    User()
        : DBMapping<3>({HANDLE_MAX_SIZE, EMAIL_MAX_SIZE, PUBLIC_KEY_MAX_SIZE}) {
    }

    ~User() = default;

    std::string_view get_handle() { return get(HANDLE); }
    std::string_view get_email() { return get(EMAIL); }
    std::string_view get_public_key() { return get(PUBLIC_KEY); }

    void set_handle(const char* handle) { set(handle, HANDLE); }
    void set_email(const char* email) { set(email, EMAIL); }
    void set_public_key(const char* public_key) { set(public_key, PUBLIC_KEY); }

   protected:
    bsoncxx::document::value serialize() {
        return make_document(kvp("handle", get_handle()),
                             kvp("email", get_email()),
                             kvp("public_key", get_public_key()));
    }

   private:
    using bson_document = bsoncxx::builder::document::kvp;
    using bsoncxx::builder::basic::make_document;

    enum PROPERTIES { HANDLE = 0, EMAIL = 1, PUBLIC_KEY = 2 };

    const uint32_t HANDLE_MAX_SIZE = 256;

    const uint32_t EMAIL_MAX_SIZE = 320;

    const uint32_t PUBLIC_KEY_MAX_SIZE = 1024;
};

class Message {};
*/

mongocxx::stdx::optional<bsoncxx::document::value> find_user_by_handle(
    const std::string& handle, const mongocxx::database& db);
