#include <fmt/core.h>

#include "http/http_parser_test.h++"
#include "http/http_request_test.h++"
#include "io/epoll_test.h++"
#include "io/inet_soc_test.h++"
#include "io/local_soc_test.h++"
#include "stl/string/ws_string_test.h++"

int main()
{
  tests::socket_inet::run();
  tests::socket_local::run();
  tests::epoll::run();
  tests::http::run();
  tests::stl::run();
  tests::http::request::run();
  return 0;
}
