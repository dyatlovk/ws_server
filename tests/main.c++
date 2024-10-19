#include <chrono>

#include "http/http_parser_test.h++"
#include "http/http_request_test.h++"
#include "http/http_uri_test.h++"
#include "http/response_test.h++"
#include "http/router_test.h++"
#include "http/stream_test.h++"
#include "io/epoll_test.h++"
#include "io/inet_soc_test.h++"
#include "io/local_soc_test.h++"
#include "stl/string/ws_string_test.h++"

int main()
{
  auto t_start = std::chrono::high_resolution_clock::now();

  tests::socket_inet::run();
  tests::socket_local::run();
  tests::epoll::run();
  tests::http::run();
  tests::stl::run();
  tests::http::request::run();
  tests::http::uri::run();
  tests::http::response::run();
  tests::http::stream::run();
  tests::http::router::run();

  auto t_end = std::chrono::high_resolution_clock::now();
  double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
  printf("==============\n");
  printf("Total duration: %f ms.\n", elapsed_time_ms);
  return 0;
}
