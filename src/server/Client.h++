#pragma once

#include <layers/TcpAbstract.h++>

namespace Server
{
  class Client : public Layers::TcpAbstract
  {
  public:
    Client(int socket);

  protected:
    auto Open() -> bool override { return false; };

    auto Bind() -> bool override { return false; }

    auto Listen(const int max) -> bool override { return false; }

    auto Accept() -> int override;

  private:

  };
} // namespace Server
