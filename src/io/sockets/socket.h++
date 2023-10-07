#pragma once

#include <fcntl.h>
#include <string>
#include <sys/socket.h>

namespace io
{
  class socket
  {
  public:
    enum class states : uint8_t
    {
      noinit = 0,
      opened = 1,
      binded = 2,
      listen = 3,
      error = 4,
      closed = 5,
    };

    virtual ~socket(){};

    /**
     * @brief      Opening(creating) fd
     * @see        man open
     *
     * @return     true on success
     */
    virtual auto open() -> bool = 0;

    /**
     * @brief      Assigns the address to the socket
     * @see        man bind
     *
     * @return     true on success
     */
    virtual auto bind() -> bool = 0;

    /**
     * @brief      Switch socket to listening mode. After this socket is ready
     *             to accepting connections
     * @see        man listen
     *
     * @param[in]  max   The maximum length of queue of pending connections
     *
     * @return     true on success
     */
    virtual auto listen(const int max) -> bool = 0;

    /**
     * @brief      Close fd
     * @see        man close
     *
     * @return     void
     */
    virtual auto close() -> void = 0;

    /**
     * @brief      Accept a new connections. A new connected socket fd was
     *             creating in a non listening mode.
     * @see        man accept
     *
     * @return     If value > 0 connected socket fd, other values is error
     */
    virtual auto accept() -> int = 0;

    /**
     * @brief      Read(receive) data from socket. This method reading all data
     *             until fd returning eof. Because system read() not guaranteed
     *             reading all data at once.
     * @see        man read
     *
     * @param[in]  conn     Read from this socket
     * @param[in]  bufSize  The buffer size of chunk
     *
     * @return     buffer
     */
    virtual auto read(const int conn, const int bufSize = 1024) -> const std::string & = 0;

    /**
     * @brief      Write(send) data to the socket
     * @see        man write
     *
     * @param[in]  conn  Connected socket.
     * @param[in]  buf   The buffer
     *
     * @return     true on send ok
     */
    virtual auto write(const int conn, const char *buf) -> bool = 0;

    /**
     * @brief      Get file descriptor
     *
     * @return     The fd.
     */
    virtual auto get_fd() -> int { return fd; };

    /**
     * @brief      Append new flags, keeping existing
     * @link #set_flags
     *
     * @param[in]  flags  The flags
     *
     * @return     void
     */
    virtual auto append_flags(const int flags) -> void
    {
      int old = get_flags();
      set_flags(old | flags);
    }

    /**
     * @brief      Set new flags, replacing existing
     * @see        man fcntl
     *
     * @param[in]  flags  The flags
     *
     * @return     void
     */
    virtual auto set_flags(const int flags) -> void { fcntl(fd, F_SETFL, flags); }

    /**
     * @brief      Removes a flags.
     * @see        man fcntl
     *
     * @param[in]  flag  The flag
     *
     * @return     void
     */
    virtual auto rm_flag(const int flag) -> void
    {
      int old = get_flags();
      int new_ = old &= ~flag;
      set_flags(new_);
    }

    /**
     * @brief      Get all flags
     * @link #set_flags
     *
     * @return     All flags.
     */
    virtual auto get_flags() -> int { return fcntl(fd, F_GETFL, 0); };

    /**
     * @brief      Get current state
     *
     * @return     Current state.
     */
    auto get_state() -> const states & { return state_; };

    /**
     * @brief      Manipulating options
     * @see        man setsockopt
     *
     * @param[in]  level  The level
     * @param[in]  opt    Option
     * @param[in]  value  New value
     *
     * @return     true on success
     */
    virtual auto set_opt(const int level, const int opt, const int value) -> bool
    {
      return setsockopt(fd, level, opt, (char *)&value, sizeof(int)) != -1;
    }

    /**
     * @brief      Get options
     * @link #set_opt
     *
     * @param[in]  lvl   The level
     * @param[in]  opt   The option
     *
     * @return     value.
     */
    virtual auto get_opt(const int lvl, const int opt) -> int
    {
      int value;
      socklen_t len = sizeof(value);
      auto result = getsockopt(fd, lvl, opt, &value, &len);

      if (result == -1)
        return -1;

      return value;
    }

    // aliases for some options
    virtual auto set_linger(const bool val, const int timeout) -> void
    {
      linger lin;
      lin.l_onoff = (int)false;
      lin.l_linger = false;
      setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&lin, sizeof(int));
    }
    virtual auto reuse_port(const bool val = false) -> void { set_opt(SOL_SOCKET, SO_REUSEPORT, (int)val); }
    virtual auto reuse_addr(const bool val = false) -> void { set_opt(SOL_SOCKET, SO_REUSEADDR, (int)val); }

  protected:
    // buffer for messages
    std::string readBuf;

    // current state
    states state_;

    // file descriptor
    int fd;

    // socket parameters
    int domain_; // AF_INET, AF_UNIX ...
    int proto_; // SOCK_STREAM, SOCK_DGRAM ...
    int type_;
  };
} // namespace io
