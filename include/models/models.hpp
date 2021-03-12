#ifndef MODELS_HPP
#define MODELS_HPP

#include <variant>

#include "mongo_odm.hpp"

class User : public DBMapping<3> {
   private:
    enum PROPERTY_CODE { HANDLE = 0, EMAIL = 1, PUBLIC_KEY = 2 };

   public:
    void set_handle(const std::string& data);
    void set_email(const std::string& data);
    void set_public_key(const std::string& data);
    std::string get_handle();
    std::string get_email();
    std::string get_public_key();

    bsoncxx::document::value serialize() override;
    void deserialize(const bsoncxx::document::view& data) override;

    static bool exists(std::string handle, Database& db);
};

class Message : public DBMapping<5> {
   private:
    enum PROPERTY_CODE {
        SENDER = 0,
        RECIEVER = 1,
        PAYLOAD = 2,
        DATETIME = 3,
        ENCRYPTED_BY = 4
    };

   public:
    void set_sender(const std::string& data);
    void set_reciever(const std::string& data);
    void set_payload(const std::string& data);
    void set_encrypted_by(const std::string& data);
    void set_datetime(const std::chrono::system_clock::time_point& data);

    std::string get_sender();
    std::string get_reciever();
    std::string get_payload();
    std::string get_encrypted_by();
    std::chrono::system_clock::time_point get_datetime();

    bsoncxx::document::value serialize() override;
    void deserialize(const bsoncxx::document::view& data) override;
};

#endif
