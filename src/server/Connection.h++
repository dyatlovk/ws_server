#pragma once

#include <string>

namespace Websock
{
  class Connection
  {
  public:
    Connection(const std::string &host, int port);

    ~Connection();

    auto SetHost(const std::string &host) -> void;

    auto SetPort(int port) -> void;

    auto GetId() -> int { return id; };

    // -------------------------------------------------------------------------
    // Mark connection for disconnecting
    auto Disconnect() -> void { isDisconnect = true; };

    // -------------------------------------------------------------------------
    // Check if connection have disconnect flag
    auto isShouldDisconnected() -> bool { return isDisconnect; };

  private:
    // -------------------------------------------------------------------------
    // Generate client uniq id
    auto GenerateId() -> int;

  private:
    int port;
    std::string host;
    bool isDisconnect;
    int id;
  };
} // namespace Websock
