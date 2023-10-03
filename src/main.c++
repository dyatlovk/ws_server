#include <server/Server.h++>

int main()
{
  auto Server = new Websock::Server(3044);
  auto result = Server->Listen();
  delete Server;
  return result;
}
