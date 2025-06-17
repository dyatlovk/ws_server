#pragma once

#include <cstdint>
#include <fcntl.h>
#include <string>
#include <sys/socket.h>

namespace io
{
  /**
   * @brief      Abstract base class for socket implementations
   *
   * @details    This class provides a common interface for different types of sockets
   *             (TCP, UDP, Unix domain sockets, etc.). It encapsulates the fundamental
   *             socket operations like opening, binding, listening, accepting connections,
   *             and data I/O operations. The class follows RAII principles and provides
   *             both low-level socket operations and high-level convenience methods.
   *
   * @note       This is an abstract class - implementations must override the pure
   *             virtual methods to provide socket-type-specific behavior.
   *
   * @author     WebSocket Server Project
   * @version    1.0
   * @since      C++17
   */
  class socket
  {
  public:
    /**
     * @brief      Socket state enumeration
     *
     * @details    Represents the current operational state of the socket.
     *             States follow the typical socket lifecycle: noinit -> opened ->
     *             binded -> listen -> (accepting connections) -> closed.
     *             The error state can occur at any point if an operation fails.
     */
    enum class states : uint8_t
    {
      noinit = 0, ///< Socket not initialized (default state)
      opened = 1, ///< Socket file descriptor created successfully
      binded = 2, ///< Socket bound to a specific address/port
      listen = 3, ///< Socket is listening for incoming connections
      error = 4,  ///< Socket encountered an error during operation
      closed = 5, ///< Socket has been closed and is no longer usable
    };

    /**
     * @brief      Virtual destructor
     *
     * @details    Ensures proper cleanup of derived socket implementations.
     *             Derived classes should handle proper socket closure and
     *             resource cleanup in their destructors.
     */
    virtual ~socket(){};

    /**
     * @brief      Create and initialize the socket file descriptor
     * @see        man socket(2)
     *
     * @details    Creates a new socket of the appropriate type (TCP, UDP, etc.)
     *             and initializes the file descriptor. This is typically the
     *             first operation performed on a socket.
     *
     * @return     true if socket creation was successful, false otherwise
     *
     * @note       After successful open(), socket state should be 'opened'
     * @throws     May throw system exceptions on failure (implementation dependent)
     */
    virtual auto open() -> bool = 0;

    /**
     * @brief      Bind socket to a specific address and port
     * @see        man bind(2)
     *
     * @details    Associates the socket with a specific network address and port.
     *             For server sockets, this defines which address/port the server
     *             will listen on. The actual address is implementation-specific
     *             and set during socket construction.
     *
     * @return     true if binding was successful, false otherwise
     *
     * @pre        Socket must be in 'opened' state
     * @post       On success, socket state becomes 'binded'
     * @note       Common failure reasons: address already in use, insufficient permissions
     */
    virtual auto bind() -> bool = 0;

    /**
     * @brief      Enable listening mode for incoming connections
     * @see        man listen(2)
     *
     * @details    Puts the socket into passive mode, ready to accept incoming
     *             connection requests. Only applicable to connection-oriented
     *             socket types (e.g., TCP sockets).
     *
     * @param[in]  max   The maximum length of the queue for pending connections.
     *                   If a connection request arrives when the queue is full,
     *                   the client may receive ECONNREFUSED error.
     *
     * @return     true if listen mode was successfully enabled, false otherwise
     *
     * @pre        Socket must be in 'binded' state
     * @post       On success, socket state becomes 'listen'
     * @note       Typical max values range from 5 to SOMAXCONN (system-dependent)
     */
    virtual auto listen(const int max) -> bool = 0;

    /**
     * @brief      Close the socket and release resources
     * @see        man close(2)
     *
     * @details    Closes the socket file descriptor and releases all associated
     *             system resources. Any pending data may be lost. After closing,
     *             the socket cannot be reused.
     *
     * @post       Socket state becomes 'closed'
     * @note       This method should be idempotent (safe to call multiple times)
     */
    virtual auto close() -> void = 0;

    /**
     * @brief      Accept a new incoming connection
     * @see        man accept(2)
     *
     * @details    Blocks until a new connection request arrives, then creates
     *             a new connected socket for that client. The original socket
     *             remains in listening mode for additional connections.
     *
     * @return     On success: file descriptor of the new connected socket (> 0)
     *             On error: negative value indicating the error condition
     *
     * @pre        Socket must be in 'listen' state
     * @note       This is a blocking operation unless socket is configured as non-blocking
     * @warning    Returned file descriptor must be properly managed and closed
     */
    virtual auto accept() -> int = 0;

    /**
     * @brief      Read data from a connected socket
     * @see        man read(2), man recv(2)
     *
     * @details    Reads all available data from the specified connection until
     *             EOF or error occurs. Unlike the system read() call, this method
     *             continues reading until all data is received, as TCP can split
     *             data across multiple packets.
     *
     * @param[in]  conn     File descriptor of the connected socket to read from
     * @param[in]  bufSize  Size of internal buffer chunks used for reading (bytes)
     *                      Default: 1024 bytes. Larger values may improve performance
     *                      for high-volume data transfer.
     *
     * @return     Reference to internal buffer containing the received data
     *
     * @note       The returned reference may become invalid after subsequent read operations
     * @warning    Large bufSize values consume more memory but may reduce system calls
     */
    virtual auto read(const int conn, const int bufSize = 1024) -> const std::string & = 0;

    /**
     * @brief      Send data to a connected socket
     * @see        man write(2), man send(2)
     *
     * @details    Sends the specified data buffer to the connected socket.
     *             This method attempts to send all data in a single operation.
     *             For large data transfers, consider using write_chunked() instead.
     *
     * @param[in]  conn  File descriptor of the connected socket
     * @param[in]  buf   Pointer to the data buffer to send
     * @param[in]  size  Number of bytes to send from the buffer
     *
     * @return     true if all data was sent successfully, false on error
     *
     * @note       Partial writes are handled internally - method succeeds only
     *             if all requested bytes are transmitted
     * @warning    Buffer must remain valid for the duration of the call
     */
    virtual auto write(const int conn, const char *buf, int size) -> bool = 0;

    /**
     * @brief      Send data using chunked transfer
     * @see        write()
     *
     * @details    Sends large data buffers by splitting them into smaller chunks.
     *             This approach is more memory-efficient for large transfers and
     *             provides better flow control. Useful for streaming data or
     *             when dealing with memory constraints.
     *
     * @param[in]  conn   File descriptor of the connected socket
     * @param[in]  buf    Pointer to the data buffer to send
     * @param[in]  size   Total number of bytes to send from the buffer
     *
     * @return     Number of bytes actually sent (may be less than requested size on error)
     *
     * @note       Return value of 0 indicates connection closed by peer
     * @note       Return value < size indicates partial transmission or error
     */
    virtual auto write_chunked(const int conn, const char *buf, int size) -> int = 0;

    /**
     * @brief      Get the socket file descriptor
     *
     * @details    Returns the underlying file descriptor used by this socket.
     *             Useful for integration with low-level APIs, select/poll/epoll
     *             operations, or when socket fd needs to be passed to other functions.
     *
     * @return     The socket file descriptor, or -1 if socket is not initialized
     *
     * @warning    Direct manipulation of the fd may interfere with socket state management
     * @note       The fd becomes invalid after close() is called
     */
    virtual auto get_fd() -> int { return fd; };

    /**
     * @brief      Add flags to the socket without removing existing ones
     * @see        set_flags(), get_flags(), man fcntl(2)
     *
     * @details    Performs a bitwise OR operation with existing flags and the
     *             provided flags. This allows enabling additional socket properties
     *             while preserving current settings.
     *
     * @param[in]  flags  Socket flags to add (e.g., O_NONBLOCK, O_CLOEXEC)
     *
     * @note       Common flags: O_NONBLOCK (non-blocking I/O), O_CLOEXEC (close on exec)
     */
    virtual auto append_flags(const int flags) -> void
    {
      int old = get_flags();
      set_flags(old | flags);
    }

    /**
     * @brief      Set socket flags, replacing all existing flags
     * @see        man fcntl(2)
     *
     * @details    Completely replaces the current socket flags with the provided
     *             value. Use append_flags() if you want to preserve existing flags.
     *
     * @param[in]  flags  New flags value to set on the socket
     *
     * @warning    This overwrites ALL existing flags - use with caution
     * @note       To remove specific flags while keeping others, use rm_flag()
     */
    virtual auto set_flags(const int flags) -> void { fcntl(fd, F_SETFL, flags); }

    /**
     * @brief      Remove specific flags from the socket
     * @see        man fcntl(2)
     *
     * @details    Removes the specified flag(s) while preserving all other
     *             currently set flags. Uses bitwise AND with complement to
     *             selectively clear flags.
     *
     * @param[in]  flag  Flag(s) to remove from the socket (e.g., O_NONBLOCK)
     *
     * @note       Multiple flags can be removed by OR-ing them together
     */
    virtual auto rm_flag(const int flag) -> void
    {
      int old = get_flags();
      int new_ = old &= ~flag;
      set_flags(new_);
    }

    /**
     * @brief      Retrieve all current socket flags
     * @see        set_flags(), man fcntl(2)
     *
     * @details    Returns the complete set of flags currently applied to
     *             the socket file descriptor.
     *
     * @return     Bitmask of all currently set flags, or -1 on error
     *
     * @note       Use bitwise AND to check for specific flags: (get_flags() & O_NONBLOCK)
     */
    virtual auto get_flags() -> int { return fcntl(fd, F_GETFL, 0); };

    /**
     * @brief      Get the current socket state
     *
     * @details    Returns the current operational state of the socket as defined
     *             by the states enumeration. This provides a high-level view of
     *             what operations are currently valid on the socket.
     *
     * @return     Reference to the current socket state
     *
     * @note       State is managed internally and updated by socket operations
     */
    auto get_state() -> const states & { return state_; };

    /**
     * @brief      Set socket option value
     * @see        man setsockopt(2)
     *
     * @details    Configures various socket-level and protocol-level options.
     *             This is a low-level interface for socket configuration.
     *             Common use cases include setting timeouts, buffer sizes,
     *             and protocol-specific options.
     *
     * @param[in]  level  Protocol level (SOL_SOCKET for socket-level options,
     *                    IPPROTO_TCP for TCP options, etc.)
     * @param[in]  opt    Option identifier (SO_REUSEADDR, SO_KEEPALIVE, etc.)
     * @param[in]  value  Option value to set
     *
     * @return     true if option was set successfully, false on error
     *
     * @note       Available options depend on the socket type and protocol
     * @example    set_opt(SOL_SOCKET, SO_REUSEADDR, 1) // Enable address reuse
     */
    virtual auto set_opt(const int level, const int opt, const int value) -> bool
    {
      return setsockopt(fd, level, opt, (char *)&value, sizeof(int)) != -1;
    }

    /**
     * @brief      Get socket option value
     * @see        man getsockopt(2)
     *
     * @details    Retrieves the current value of a socket option. Used to
     *             query socket configuration or verify option settings.
     *
     * @param[in]  lvl   Protocol level (SOL_SOCKET, IPPROTO_TCP, etc.)
     * @param[in]  opt   Option identifier to query
     *
     * @return     Current option value, or -1 on error
     *
     * @note       The returned value interpretation depends on the specific option
     */
    virtual auto get_opt(const int lvl, const int opt) -> int
    {
      int value;
      socklen_t len = sizeof(value);
      auto result = getsockopt(fd, lvl, opt, &value, &len);

      if (result == -1) return -1;

      return value;
    }

    /**
     * @brief      Configure socket linger behavior on close
     * @see        man setsockopt(2), struct linger
     *
     * @details    Controls what happens to unsent data when socket is closed.
     *             When linger is enabled, close() will block until data is sent
     *             or timeout expires. When disabled, close() returns immediately.
     *
     * @param[in]  val      Enable/disable linger behavior
     * @param[in]  timeout  Maximum time to wait for data transmission (seconds)
     *
     * @note       Currently implementation has a bug - always sets linger to disabled
     * @warning    This method needs to be fixed to properly use the parameters
     */
    virtual auto set_linger(const bool val, const int timeout) -> void
    {
      linger lin;
      lin.l_onoff = (int)false;
      lin.l_linger = false;
      setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&lin, sizeof(int));
    }

    /**
     * @brief      Enable/disable port reuse
     * @see        man setsockopt(2), SO_REUSEPORT
     *
     * @details    Allows multiple sockets to bind to the same port, enabling
     *             load balancing across multiple processes. Useful for
     *             multi-process server architectures.
     *
     * @param[in]  val  true to enable port reuse, false to disable
     *
     * @note       SO_REUSEPORT is Linux/BSD specific - not available on all systems
     */
    virtual auto reuse_port(const bool val = false) -> void { set_opt(SOL_SOCKET, SO_REUSEPORT, (int)val); }

    /**
     * @brief      Enable/disable address reuse
     * @see        man setsockopt(2), SO_REUSEADDR
     *
     * @details    Allows binding to an address that's in TIME_WAIT state.
     *             Essential for servers that need to restart quickly without
     *             waiting for socket cleanup. Prevents "Address already in use" errors.
     *
     * @param[in]  val  true to enable address reuse, false to disable
     *
     * @note       Should typically be enabled for server sockets
     */
    virtual auto reuse_addr(const bool val = false) -> void { set_opt(SOL_SOCKET, SO_REUSEADDR, (int)val); }

  protected:
    /**
     * @brief      Internal buffer for received data
     *
     * @details    Stores data read from socket connections. This buffer is reused
     *             across read operations for efficiency. The buffer grows as needed
     *             to accommodate incoming data.
     *
     * @note       Buffer contents are valid until next read operation
     */
    std::string readBuf;

    /**
     * @brief      Current operational state of the socket
     *
     * @details    Tracks the socket's lifecycle state using the states enum.
     *             Updated automatically by socket operations. Used for validation
     *             and state management.
     */
    states state_;

    /**
     * @brief      Socket file descriptor
     *
     * @details    The underlying system file descriptor for this socket.
     *             Set during open() and remains valid until close().
     *             Value of -1 indicates uninitialized socket.
     */
    int fd;

    /**
     * @brief      Socket domain/family specification
     *
     * @details    Defines the communication domain for the socket.
     *             Common values:
     *             - AF_INET: IPv4 Internet protocols
     *             - AF_INET6: IPv6 Internet protocols
     *             - AF_UNIX: Unix domain sockets (local communication)
     */
    int domain_;

    /**
     * @brief      Socket protocol specification
     *
     * @details    Specifies the protocol within the domain.
     *             Usually 0 to select default protocol for the socket type.
     *             Can be specific values like IPPROTO_TCP, IPPROTO_UDP for IP sockets.
     */
    int proto_;

    /**
     * @brief      Socket type specification
     *
     * @details    Defines the socket communication semantics.
     *             Common values:
     *             - SOCK_STREAM: Reliable, connection-oriented (TCP)
     *             - SOCK_DGRAM: Unreliable, connectionless (UDP)
     *             - SOCK_RAW: Raw protocol access
     */
    int type_;
  };
} // namespace io
