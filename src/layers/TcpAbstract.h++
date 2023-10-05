#pragma once
#include <arpa/inet.h>

namespace Layers
{
  class TcpAbstract
  {
  public:
    /**
     * Mechanism for monitoring file descriptors.
     *
     * Accept: Old & blocking. Max descriptors 1024. Crossplatform. Highres timeout
     * Poll: More then 1024 descriptors. Crossplatform
     * Epoll: More then 1024 descriptors. Linux only
     **/
    enum class AcceptType : uint8_t
    {
      Accept = 0,
      Poll = 1,
      Epoll = 2,
    };

  public:
    TcpAbstract(const AcceptType type = AcceptType::Epoll);

    virtual ~TcpAbstract();

    /**
     * Close connection
     **/
    virtual auto CloseConnection() -> void;

    auto SetPort(const int port) -> void { this->port = port; }

    auto GetPort() -> int const { return port; }

    auto GetHost() -> const char * { return host; }

    auto SetHost(const char *host) -> void { this->host = host; }

    auto GetMaxConn() -> int { return max_conn; }

  protected:
    constexpr static const unsigned int MAX_CONN = 1000;

    /**
     * Create socket
     **/
    virtual auto Open() -> bool = 0;

    /**
     * Bind a name to an opened socket
     **/
    virtual auto Bind() -> bool = 0;

    /**
     * Start listen for connection
     **/
    virtual auto Listen(const int max) -> bool = 0;

    /**
     * Accept a connection on a socket
     * */
    virtual auto Accept() -> int = 0;

  protected:
    int port;
    const char *host;
    int max_conn;
    AcceptType acceptType;

    int fd = 0;
    struct sockaddr_in addr;
  };
} // namespace Layers
