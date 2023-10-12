#include <fmt/core.h>

#include "http/http_parser_test.h++"
#include "io/epoll_test.h++"
#include "io/inet_soc_test.h++"
#include "io/local_soc_test.h++"

int main()
{
  tests::socket_inet::run();
  tests::socket_local::run();
  tests::epoll::run();
  tests::http::run();
  return 0;
}
