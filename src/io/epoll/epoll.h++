#pragma once
#include <functional>
#include <sys/epoll.h>

namespace io
{
  /**
   * IO monitoring mechanism. Linux only.
   * @see man epoll
   **/
  class Epoll
  {
  public:
    /**
     * Epoll events
     * @see man epoll_ctl
     **/
    enum Events
    {
      INCOMING = EPOLLIN,
      READ = EPOLLIN,
      WRITE = EPOLLOUT,
      CLOSE = EPOLLRDHUP,
      SHUTDOWN = EPOLLRDHUP,
    };

  public:
    explicit Epoll(const int max = MAX_EVENTS_DEFAULT);

    ~Epoll();

    /**
     * Create epoll instance
     **/
    auto Create() -> void;

    /**
     * Wait IO on epoll descriptor instance
     * @see man epoll_wait
     **/
    auto Wait() -> void;

    /**
     * Close socket descriptor
     **/
    auto Close(const int sock) -> void;

    /**
     * Add socket to monitoring by epoll
     * @param e epoll events(man epoll_ctl)
     **/
    auto RegisterSocket(const int socket, const uint32_t e = Events::INCOMING) -> void;

    /**
     * Remove socket from epoll monitoring
     **/
    auto UnregisterSocket(const int socket) -> void;

    /**
     * Get epoll fd
     */
    auto GetDescriptor() -> int { return fd; };

    /**
     * New connection or read event on a socket
     *
     * @param $1 int socket
     * @param $2 bool What this connection? True: is a master(server). False: is a slave (connected client)
     **/
    auto OnIncoming(std::function<void(int, bool)> &&callback) -> void { onIncoming = callback; };

    /**
     * Write event on a socket
     *
     * @param $1 int socket
     * @param $2 bool isMaster (@see OnIncoming params)
     **/
    auto OnWrite(std::function<void(int, bool)> &&callback) -> void { onWrite = callback; }

    /**
     * Socket closed
     *
     * @param $1 int socket
     * @param $2 bool isMaster (@see OnIncoming params)
     **/
    auto OnClose(std::function<void(int, bool)> &&callback) -> void { onClose = callback; };

    /**
     * Set master socket
     **/
    auto SetMasterSocket(const int fd) -> void { masterSocket = fd; };

    /**
     * Specifies time in ms that epoll_wait() will blocked
     */
    auto SetTimeOut(const int ms) -> void { timeout = ms; };

  private:
    constexpr static const unsigned int MAX_EVENTS_DEFAULT = 1000;
    constexpr static const int WAIT_TIMEOUT_DEFAULT = -1;

    int fd; // epoll file descriptor
    struct epoll_event *evlist, events[MAX_EVENTS_DEFAULT];
    int wait_events;
    int maxEvents;
    int timeout;
    int masterSocket;

    std::function<void(int, bool)> onIncoming;
    std::function<void(int, bool)> onWrite;
    std::function<void(int, bool)> onClose;
  };
} // namespace io
