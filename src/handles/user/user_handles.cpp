#include "../../../includes/handles/user/user_handles.hpp"

crow::json::wvalue FindUserHandle(crow::json::wvalue user_handle) {
  std::cout << user_handle["handle"].dump() << std::endl;

  crow::json::wvalue response;
  response["status"] = "OK";
  response["public_key"] = "key";
  return response;
}
