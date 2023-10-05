#pragma once

namespace Server
{
  class ServerAbstract
  {
  public:
    ServerAbstract() = default;

    virtual ~ServerAbstract() = default;

    virtual auto Run() -> void = 0;

    virtual auto ShutDown() -> void = 0;
  };
} // namespace Server
