#include "Connection.h++"

namespace Websock
{
  Connection::Connection(const std::string &host, int port)
      : port(port)
      , host(host)
      , id(0)
  {
    this->id = GenerateId();
  }

  Connection::~Connection() = default;

  auto Connection::SetHost(const std::string &host) -> void
  {
    this->host = host;
  }

  auto Connection::SetPort(int port) -> void
  {
    this->port = port;
  }

  auto Connection::GenerateId() -> int
  {
    return rand() % 100;
  }
} // namespace Websock
