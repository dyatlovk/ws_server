#pragma once

#include <functional>
#include <string>
#include <sys/epoll.h>
#include <unordered_set>

namespace io
{
  /**
   * @brief      IO monitoring mechanism. Linux only.
   * @see        man epoll
   */
  class epoll
  {
  public:
    /**
     * @brief      Supported events
     * @see        man epoll_ctl
     */
    enum Events
    {
      INCOMING = EPOLLIN,
      READ = EPOLLIN,
      WRITE = EPOLLOUT,
      CLOSE = EPOLLRDHUP,
      SHUTDOWN = EPOLLRDHUP,
    };

  public:
    explicit epoll(const int max = MAX_EVENTS_DEFAULT);

    ~epoll();

    /**
     * @brief      Create epoll instance
     *
     * @return     void
     * @throws     std::runtime_error  Error create
     */
    auto create() -> void;

    /**
     * @brief      Wait IO on epoll descriptor instance
     * @see        man epoll_wait
     *
     * @return     void
     * @throws     std::runtime_error  Wait error or master socket not defined
     */
    auto wait() -> void;

    /**
     * @brief      Close epoll and connected sockets
     *
     * @return     void
     */
    auto shutdown() -> void;

    /**
     * @brief      Add socket to monitoring by epoll. Master socket is protected
     *
     * @param[in]  socket  The socket
     * @param      e       epoll events(man epoll_ctl)
     *
     * @return     void
     */
    auto watch(const int socket, const uint32_t e = EPOLLIN | EPOLLET | EPOLLRDHUP) -> void;

    /**
     * @brief      Master socket watching clients connections. One socket per instance
     *
     * @param[in]  socket  The socket
     * @param[in]  e       epoll events(man epoll_ctl)
     *
     * @return     void
     */
    auto register_master(const int socket, const uint32_t e = EPOLLIN) -> void;

    /**
     * @brief      Remove socket from epoll monitoring and closing it. Master
     *             socket is protected
     *
     * @param[in]  socket  The socket
     *
     * @return     void
     */
    auto unwatch(const int socket) -> void;

    /**
     * @brief      Like an unwatch method, but for all clients
     *
     * @return     void
     */
    auto unwatch_all() -> void;

    /**
     * @brief      Get size watched clients
     *
     * @return     int count
     */
    auto watched_size() -> int { return watched_.size(); };

    /**
     * @brief      Get epoll fd
     *
     * @return     Epoll fd descriptor.
     */
    auto get_fd() -> int { return fd; };

    /**
     * @brief      Any incoming connection (client or server)
     *
     * @param      callback  The callback
     * @param      int   socket
     * @param      bool  What a type of this connection? True: is a master(server).
     *                   False: is a slave (connected client)
     *
     * @return     void
     */
    auto on_incoming(std::function<void(int, bool)> &&callback) -> void { on_incoming_ = callback; };

    /**
     * @brief      A new client connection event
     *
     * @param      callback  The callback
     *
     * @return     void
     */
    auto on_connection(std::function<void(int)> &&callback) -> void { on_connection_ = callback; };

    /**
     * @brief      Write event on a socket (client sending data)
     *
     * @param      callback  The callback
     * @param      int   socket
     *
     * @return     void
     */
    auto on_write(std::function<void(int, const char *)> &&callback) -> void { on_write_ = callback; }

    /**
     * @brief      Socket closed (client is closed connection)
     *
     * @param      callback  The callback
     * @param      int   socket
     *
     * @return     void
     */
    auto on_close(std::function<void(int)> &&callback) -> void { on_close_ = callback; };

    /**
     * @brief      Specifies time in ms that epoll_wait() will be blocked
     *
     * @param[in]  ms    timeout in ms
     *
     * @return     void
     */
    auto set_timeout(const int ms) -> void { timeout = ms; };

  private:
    /**
     * @brief      Accept a new peer
     *
     * @return     Socket fd
     */
    auto peer_accept() -> int;

    /**
     * @brief      Peer sending data, reading it
     *
     * @param[in]  peer     Socket fd
     * @param[in]  bufSize  Size of chunks
     *
     * @return     readed data
     */
    auto peer_read(const int peer, const int bufSize = 1024) -> const char *;

    auto add(const int socket, const uint32_t e) -> int;

    auto remove(const int socket) -> int;

  private:
    constexpr static const unsigned int MAX_EVENTS_DEFAULT = 0x3e8; // 1000
    constexpr static const int WAIT_TIMEOUT_DEFAULT = 0x1; // 1

    int fd; // epoll file descriptor
    struct epoll_event evlist, events_[MAX_EVENTS_DEFAULT];
    int maxEvents;
    int timeout;
    int masterSocket;
    std::string readBuf; // send data buffer

    std::function<void(int, bool)> on_incoming_;
    std::function<void(int)> on_connection_;
    std::function<void(int, const char *)> on_write_;
    std::function<void(int)> on_close_;

    std::unordered_set<int> watched_;
  };
} // namespace io
