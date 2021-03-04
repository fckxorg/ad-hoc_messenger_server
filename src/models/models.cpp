#include "models.hpp"

//============================================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                      User Database Model
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//============================================================================
using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

void User::set_handle(const std::string& data) { set(data, HANDLE); }
void User::set_email(const std::string& data) { set(data, EMAIL); }
void User::set_public_key(const std::string& data) { set(data, PUBLIC_KEY); }
std::string User::get_handle() { return std::get<STRING>(get(HANDLE)); }
std::string User::get_email() { return std::get<STRING>(get(EMAIL)); }
std::string User::get_public_key() { return std::get<STRING>(get(PUBLIC_KEY)); }

bsoncxx::document::value User::serialize() {
    return make_document(kvp("handle", get_handle()), kvp("email", get_email()),
                         kvp("public_key", get_public_key()));
}

void User::deserialize(const bsoncxx::document::view& data) {
    // TODO erorr-checking
    set_handle(get_string_from_bson(data, "handle"));
    set_email(get_string_from_bson(data, "email"));
    set_public_key(get_string_from_bson(data, "public_key"));
}

//============================================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                      Message Database Model
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//============================================================================

void Message::set_sender(const std::string& data) { set(data, SENDER); }
void Message::set_reciever(const std::string& data) { set(data, RECIEVER); }
void Message::set_payload(const std::string& data) { set(data, PAYLOAD); }
void Message::set_encrypted_by(const std::string& data) {
    set(data, ENCRYPTED_BY);
}
void Message::set_datetime(const std::chrono::system_clock::time_point& data) {
    set(data, DATETIME);
}
std::string Message::get_sender() { return std::get<STRING>(get(SENDER)); }
std::string Message::get_reciever() { return std::get<STRING>(get(RECIEVER)); }
std::string Message::get_payload() { return std::get<STRING>(get(PAYLOAD)); }
std::string Message::get_encrypted_by() {
    return std::get<STRING>(get(ENCRYPTED_BY));
}
std::chrono::system_clock::time_point Message::get_datetime() {
    return std::get<TIME>(get(DATETIME));
}

bsoncxx::document::value Message::serialize() {
    bsoncxx::types::b_date bson_date{get_datetime()};
    return make_document(
        kvp("sender", get_sender()), kvp("reciever", get_reciever()),
        kvp("payload", get_payload()), kvp("encrypted_by", get_encrypted_by()),
        kvp("datetime", bson_date));
}

void Message::deserialize(const bsoncxx::document::view& data) {
    // TODO datetime gathering
    // TODO error-checking

    set_sender(get_string_from_bson(data, "sender"));
    set_reciever(get_string_from_bson(data, "reciever"));
    set_payload(get_string_from_bson(data, "paylaod"));
    set_encrypted_by(get_string_from_bson(data, "encrypted_by"));
}
