#include <fmt/core.h>

#include "io/inet_soc_test.h++"
#include "io/local_soc_test.h++"

int main()
{
  tests::socket_inet::run();
  tests::socket_local::run();
  return 0;
}
