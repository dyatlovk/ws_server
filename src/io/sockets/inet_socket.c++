#include "inet_socket.h++"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>

#include "../../utils/logger.h++"

namespace io
{
  inet_socket::inet_socket(const char *host, const int port, address_support address, type_support type)
      : host_(host ? std::string(host) : "") // Store copy to avoid dangling pointer
      , port_(port)
  {
    LOG_DEBUG("Creating inet_socket for {}:{}", host ? host : "NULL", port);

    // Validate input parameters
    if (!host || port <= 0 || port > 65535)
    {
      LOG_ERROR("Invalid parameters: host={}, port={}", host ? host : "NULL", port);
      state_ = states::error;
      fd = -1;
      return;
    }

    LOG_DEBUG("Socket parameters validated successfully");
    state_ = states::noinit;
    fd = -1;
    domain_ = static_cast<typename std::underlying_type<address_support>::type>(address);
    type_ = static_cast<typename std::underlying_type<type_support>::type>(type);
    proto_ = 0;
    readBuf.clear();

    // Initialize address structure to zero
    std::memset(&addr_, 0, sizeof(addr_));
    LOG_DEBUG("Socket initialized with domain={}, type={}", domain_, type_);
  }

  inet_socket::~inet_socket()
  {
    LOG_DEBUG("Destroying inet_socket for {}:{}", host_, port_);
    close();
  }

  auto inet_socket::open() -> bool
  {
    if (state_ >= states::opened) {
      LOG_DEBUG("Socket already opened for {}:{}", host_, port_);
      return true;
    }

    // Check if we're in error state from constructor
    if (state_ == states::error) {
      LOG_ERROR("Cannot open socket: in error state");
      return false;
    }

    LOG_INFO("Opening socket for {}:{} (domain={}, type={})", host_, port_, domain_, type_);

    fd = ::socket(domain_, type_, proto_);
    if (fd == -1)
    {
      LOG_ERROR("Failed to create socket: {}", std::strerror(errno));
      state_ = states::error;
      return false;
    }

    LOG_DEBUG("Socket created successfully with fd={}", fd);

    // Initialize address structure based on domain
    if (domain_ == AF_INET)
    {
      addr_.inet4_addr.sin_family = domain_;
      update_addr_host(host_.c_str());
      update_addr_port(port_);
      LOG_DEBUG("IPv4 address configured");
    }
    else if (domain_ == AF_INET6)
    {
      addr_.inet6_addr.sin6_family = domain_;
      update_addr_host(host_.c_str());
      update_addr_port(port_);
      LOG_DEBUG("IPv6 address configured");
    }
    else
    {
      LOG_ERROR("Unsupported address family: {}", domain_);
      state_ = states::error;
      ::close(fd);
      fd = -1;
      return false;
    }

    state_ = states::opened;
    LOG_INFO("Socket opened successfully for {}:{}", host_, port_);
    return true;
  }

  auto inet_socket::bind() -> bool
  {
    if (state_ >= states::binded) {
      LOG_DEBUG("Socket already bound for {}:{}", host_, port_);
      return true;
    }

    LOG_INFO("Binding socket to {}:{}", host_, port_);

    const struct sockaddr *addr_ptr;
    socklen_t addr_len;

    if (domain_ == AF_INET)
    {
      addr_ptr = reinterpret_cast<const struct sockaddr *>(&addr_.inet4_addr);
      addr_len = sizeof(addr_.inet4_addr);
      LOG_DEBUG("Using IPv4 address structure");
    }
    else if (domain_ == AF_INET6)
    {
      addr_ptr = reinterpret_cast<const struct sockaddr *>(&addr_.inet6_addr);
      addr_len = sizeof(addr_.inet6_addr);
      LOG_DEBUG("Using IPv6 address structure");
    }
    else
    {
      LOG_ERROR("Invalid address family for bind: {}", domain_);
      state_ = states::error;
      return false;
    }

    const int b = ::bind(fd, addr_ptr, addr_len);

    if (b == -1)
    {
      LOG_ERROR("Failed to bind socket to {}:{}: {}", host_, port_, std::strerror(errno));
      state_ = states::error;
      return false;
    }

    state_ = states::binded;
    LOG_INFO("Socket bound successfully to {}:{}", host_, port_);
    return true;
  }

  auto inet_socket::listen(const int max) -> bool
  {
    if (state_ >= states::listen) {
      LOG_DEBUG("Socket already listening on {}:{}", host_, port_);
      return true;
    }

    LOG_INFO("Starting to listen on {}:{} with backlog={}", host_, port_, max);

    if (::listen(fd, max) == -1)
    {
      LOG_ERROR("Failed to listen on {}:{}: {}", host_, port_, std::strerror(errno));
      state_ = states::error;
      return false;
    }

    state_ = states::listen;
    LOG_INFO("Socket listening successfully on {}:{}", host_, port_);
    return true;
  }

  auto inet_socket::close() -> void
  {
    if (fd != -1)
    {
      LOG_DEBUG("Closing socket fd={} for {}:{}", fd, host_, port_);
      ::close(fd);
      fd = -1;
    }
    state_ = states::closed;
    LOG_DEBUG("Socket closed for {}:{}", host_, port_);
  }

  auto inet_socket::accept() -> int
  {
    LOG_DEBUG("Accepting connection on {}:{}", host_, port_);

    struct sockaddr peer_addr;
    socklen_t peer_len = sizeof(peer_addr);
    const int peer = ::accept(fd, &peer_addr, &peer_len);

    if (peer == -1)
    {
      if (errno != EAGAIN && errno != EWOULDBLOCK)
      {
        LOG_ERROR("Accept failed on {}:{}: {}", host_, port_, std::strerror(errno));
        state_ = states::error;
      }
      else
      {
        LOG_DEBUG("Accept would block on {}:{}", host_, port_);
      }
      return -1;
    }

    LOG_INFO("Accepted connection from client (fd={})", peer);
    return peer;
  }

  auto inet_socket::read(const int conn, const int bufSize) -> const std::string &
  {
    readBuf.clear();
    LOG_DEBUG("Reading from connection fd={}, bufSize={}", conn, bufSize);

    // Validate parameters
    if (conn < 0 || bufSize <= 0)
    {
      LOG_WARN("Invalid read parameters: conn={}, bufSize={}", conn, bufSize);
      return readBuf; // Return empty string for invalid parameters
    }

    // Limit buffer size to prevent stack overflow
    const int maxBufSize = std::min(bufSize, 8192);
    if (maxBufSize < bufSize) {
      LOG_DEBUG("Buffer size limited from {} to {}", bufSize, maxBufSize);
    }

    // Use heap allocation for buffer to avoid stack overflow
    std::unique_ptr<char[]> buf(new char[maxBufSize]);
    std::memset(buf.get(), 0, maxBufSize);

    int byte_count = 0;
    int total_bytes = 0;
    // read from socket until EOF
    while ((byte_count = ::read(conn, buf.get(), maxBufSize)) > 0)
    {
      readBuf.append(buf.get(), byte_count);
      total_bytes += byte_count;
    }

    if (byte_count == -1) {
      LOG_ERROR("Read error on fd={}: {}", conn, std::strerror(errno));
    } else {
      LOG_DEBUG("Read {} bytes from fd={}", total_bytes, conn);
    }

    return readBuf;
  }

  auto inet_socket::write(const int conn, const char *buf, int size) -> bool
  {
    LOG_DEBUG("Writing {} bytes to connection fd={}", size, conn);

    // Validate parameters
    if (conn < 0 || !buf || size <= 0)
    {
      LOG_WARN("Invalid write parameters: conn={}, buf={}, size={}",
               conn, static_cast<const void*>(buf), size);
      return false;
    }

    int total_sent = 0;
    while (total_sent < size)
    {
      int r = ::write(conn, buf + total_sent, size - total_sent);

      if (r == -1)
      {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
          LOG_DEBUG("Write would block on fd={} after {} bytes", conn, total_sent);
          // For non-blocking sockets, return false to indicate partial failure
          return false;
        }
        LOG_ERROR("Write error on fd={}: {}", conn, std::strerror(errno));
        // For other errors, don't close the server socket
        // The caller should handle connection closure
        return false;
      }

      if (r == 0)
      {
        LOG_WARN("Connection closed by peer on fd={}", conn);
        // Connection closed by peer
        return false;
      }

      total_sent += r;
      LOG_DEBUG("Sent {} bytes to fd={} (total: {}/{})", r, conn, total_sent, size);
    }

    LOG_DEBUG("Successfully wrote all {} bytes to fd={}", size, conn);
    return true; // All data sent successfully
  }

  auto inet_socket::write_chunked(const int conn, const char *buf, int size) -> int
  {
    // Validate parameters
    if (conn < 0 || !buf || size <= 0)
    {
      return -1;
    }

    int total_sent = 0;
    while (size > 0)
    {
      int num_sent = ::write(conn, buf, size);
      if (num_sent == -1)
      {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
          // Non-blocking socket would block, return how much we've sent so far
          return total_sent;
        }
        return -1; // Error occurred
      }
      if (num_sent == 0)
      {
        // Connection closed by peer
        break;
      }
      buf += num_sent;
      size -= num_sent;
      total_sent += num_sent;
    }

    return total_sent;
  }

  auto inet_socket::set_non_blocking() -> void
  {
    append_flags(O_NONBLOCK);
  }

  auto inet_socket::make_tcp(const char *h, const int p) -> std::unique_ptr<inet_socket>
  {
    if (!h || p <= 0 || p > 65535)
    {
      return nullptr;
    }
    return std::make_unique<inet_socket>(h, p, address_support::ip4, type_support::tcp);
  }

  auto inet_socket::make_tcp6(const char *h, const int p) -> std::unique_ptr<inet_socket>
  {
    if (!h || p <= 0 || p > 65535)
    {
      return nullptr;
    }
    return std::make_unique<inet_socket>(h, p, address_support::ip6, type_support::tcp);
  }

  auto inet_socket::make_udp(const char *h, const int p) -> std::unique_ptr<inet_socket>
  {
    if (!h || p <= 0 || p > 65535)
    {
      return nullptr;
    }
    return std::make_unique<inet_socket>(h, p, address_support::ip4, type_support::udp);
  }

  auto inet_socket::make_udp6(const char *h, const int p) -> std::unique_ptr<inet_socket>
  {
    if (!h || p <= 0 || p > 65535)
    {
      return nullptr;
    }
    return std::make_unique<inet_socket>(h, p, address_support::ip6, type_support::udp);
  }

  // ---------------------------------------------------------------------------
  // PRIVATE FUNCTIONS
  // ---------------------------------------------------------------------------
  auto inet_socket::update_addr_host(const char *h) -> void
  {
    if (!h)
    {
      state_ = states::error;
      return;
    }

    int result;
    if (domain_ == AF_INET)
    {
      result = inet_pton(AF_INET, h, &(addr_.inet4_addr.sin_addr));
    }
    else if (domain_ == AF_INET6)
    {
      result = inet_pton(AF_INET6, h, &(addr_.inet6_addr.sin6_addr));
    }
    else
    {
      state_ = states::error;
      return;
    }

    if (result <= 0)
    {
      state_ = states::error;
    }
  }

  auto inet_socket::update_addr_port(const int p) -> void
  {
    if (p <= 0 || p > 65535)
    {
      state_ = states::error;
      return;
    }

    if (domain_ == AF_INET)
    {
      addr_.inet4_addr.sin_port = htons(static_cast<uint16_t>(p));
    }
    else if (domain_ == AF_INET6)
    {
      addr_.inet6_addr.sin6_port = htons(static_cast<uint16_t>(p));
    }
    else
    {
      state_ = states::error;
    }
  }
} // namespace io
