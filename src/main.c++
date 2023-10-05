#include <exception>
#include <fmt/color.h>
#include <server/TcpServer.h++>

int main()
{
  auto Server = new Server::TcpServer("127.0.0.1", 3044);
  try
  {
    Server->Run();
  }
  catch (const std::exception &e)
  {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Runtime error: {} \n", e.what());
  }

  Server->ShutDown();
  delete Server;
  return 0;
}
