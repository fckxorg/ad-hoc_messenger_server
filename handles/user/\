#include "user_handles.hpp"

crow::json::wvalue FindUserHandle(crow::json::wvalue user_handle) {
  std::cout << user_handle["handle"] << std::endl;

  crow::json::wvalue response;
  response["status"] = "OK";
  response["public_key"] = "key";
  return response;
}
