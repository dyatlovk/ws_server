#pragma once
#include <arpa/inet.h>
#include <functional>
#include <sys/epoll.h>

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

    enum class ReadStates : uint8_t
    {
      Activity = 1,
      Empty = 0,
    };

  private:
    struct SocketFD;
    SocketFD *sockFd;
    constexpr static const unsigned int MAX_EVENTS = 50;

  public:
    /**
     * Create new connection
     **/
    Tcp(const char *host, const int port, const int max = 1);

    /**
     * Make from connection
     **/
    Tcp(int socket);

    ~Tcp();

    // ---------------------------------------------------------------------------
    // Create socket
    auto Open() -> bool;

    // ---------------------------------------------------------------------------
    // Bind a name to a opened socket
    auto Bind() -> bool;

    // ---------------------------------------------------------------------------
    // Start listen for connection
    auto Listen(int max) -> bool;

    /**
     * Call all sockets routine in on function
     **/
    auto Init() -> bool;

    /**
     * Accept a connection on a socket
     * */
    auto Accept() -> int;

    /**
     * Processing a new connections. This is a server routine.
     **/
    auto ListenConnection() -> int;

    /**
     * Processing a data from sockets. This is a client routine.
     **/
    auto ListenData() -> int;

    auto IsDisconnected() -> int;

    auto Close(const int socket) -> bool;

    auto GetDescriptor() -> SocketFD * { return sockFd; }

    auto GetPort() -> int const { return port; }

    auto GetHost() -> const char * { return host; }

    auto GetMaxConn() -> int { return max_conn; }

    auto GetState() -> State *const { return &state; }

    auto CreateEpoll() -> bool;

    /**
     * Return socket descriptor on close
     **/
    auto OnClose() -> int;

    auto SetServerSocket(const int socket) -> void { serverSocket = socket; }

  private:
    struct SocketFD
    {
      int fd = 0; // File descriptor
      int fd_len = 0; // File descriptor length
      struct sockaddr_in addr;
      unsigned int socket_len = 0;
    };

    auto SetNonBlocking(const int socket) -> void;

    /**
     * Shutdown peer immediately after close socket
     * Using linger
     **/
    auto CloseImmediately(const int socket) -> void;

  private:
    int port;
    const char *host;
    int max_conn;
    State state;
    int epfd; // epoll instance
    struct epoll_event *evlist, events[MAX_EVENTS];
    int wait_events;
    int serverSocket;
  };
} // namespace Layers
