#pragma once
#include <arpa/inet.h>

namespace Layers
{
  class Tcp
  {
  public:
    enum class State : uint8_t
    {
      Up = 0,
      ErrOpen = 1,
      ErrInit = 2,
      ErrBind = 3,
      ErrListening = 4,
      Close = 5
    };

  private:
    struct SocketFD;
    SocketFD *sockFd;

  public:
    Tcp(const char *host, const int port, const int max = 1);

    ~Tcp();
    auto Open() -> bool;

    // ---------------------------------------------------------------------------
    // Bind a name to a opened socket
    auto Bind() -> bool;

    // ---------------------------------------------------------------------------
    // Start listen for connection
    auto Listen(int max) -> bool;

    // ---------------------------------------------------------------------------
    // Call all sockets routine in on function
    auto Init() -> bool;

    // ---------------------------------------------------------------------------
    // Accept a connection on a socket
    auto Accept() -> int;

    auto Close() -> bool;

    auto GetDescriptor() -> SocketFD * { return sockFd; };

    auto GetPort() -> int const { return port; }

    auto GetHost() -> const char * { return host; }

    auto GetMaxConn() -> int;

    auto GetState() -> State *const { return &state; };

  private:
    struct SocketFD
    {
      int fd = 0; // File descriptor
      int fd_len = 0; // File descriptor length
      struct sockaddr_in addr;
      unsigned int socket_len = 0;
    };

  private:
    int port;
    const char *host;
    int max_conn;
    State state;
  };
} // namespace Layers
