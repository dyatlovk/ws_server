#include "server/simple_server.h++"

int main()
{
  ::examples::server srv;
  srv.run();
  srv.shutdown();

  return 0;
}
