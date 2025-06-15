#include "inet_socket.h++"

#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

namespace io
{
  inet_socket::inet_socket(const char *host, const int port, address_support address, type_support type)
      : host_(host)
      , port_(port)
  {
    // Validate input parameters
    if (!host || port <= 0 || port > 65535)
    {
      state_ = states::error;
      fd = -1;
      return;
    }

    state_ = states::noinit;
    fd = -1;
    domain_ = static_cast<typename std::underlying_type<address_support>::type>(address);
    type_ = static_cast<typename std::underlying_type<type_support>::type>(type);
    proto_ = 0;
    readBuf.clear();
  }

  inet_socket::~inet_socket()
  {
    close();
  }

  auto inet_socket::open() -> bool
  {
    if (state_ >= states::opened) return true;

    // Check if we're in error state from constructor
    if (state_ == states::error) return false;

    fd = ::socket(domain_, type_, proto_);
    if (fd == -1)
    {
      state_ = states::error;
      return false;
    }

    inet_addr.sin_family = domain_;
    update_addr_host(host_);
    update_addr_port(port_);

    state_ = states::opened;

    return true;
  }

  auto inet_socket::bind() -> bool
  {
    if (state_ >= states::binded) return true;

    const int b = ::bind(fd, (const struct sockaddr *)&inet_addr, sizeof(inet_addr));

    if (b == -1)
    {
      state_ = states::error;
      return false;
    }

    state_ = states::binded;

    return true;
  }

  auto inet_socket::listen(const int max) -> bool
  {
    if (state_ >= states::listen) return true;

    if (::listen(fd, max) == -1)
    {
      state_ = states::error;
      return false;
    }

    state_ = states::listen;

    return true;
  }

  auto inet_socket::close() -> void
  {
    if (fd != -1)
    {
      ::close(fd);
      fd = -1;
    }
    state_ = states::closed;
  }

  auto inet_socket::accept() -> int
  {
    struct sockaddr peer_addr;
    socklen_t peer_len = sizeof(peer_addr);
    const int peer = ::accept(fd, &peer_addr, &peer_len);

    if (peer == -1)
    {
      if (errno != EAGAIN && errno != EWOULDBLOCK)
      {
        state_ = states::error;
      }
      return -1;
    }

    return peer;
  }

  auto inet_socket::read(const int conn, const int bufSize) -> const std::string &
  {
    readBuf.clear();

    // Validate parameters
    if (conn < 0 || bufSize <= 0)
    {
      return readBuf; // Return empty string for invalid parameters
    }

    char buf[bufSize];
    memset(buf, 0, sizeof(buf));

    int byte_count = 0;
    // read from socket until EOF
    while ((byte_count = ::read(conn, buf, bufSize)) > 0)
    {
      readBuf.append(buf, byte_count);
    }

    return readBuf;
  }

  auto inet_socket::write(const int conn, const char *buf, int size) -> bool
  {
    // Validate parameters
    if (conn < 0 || !buf || size <= 0)
    {
      return false;
    }

    int r = ::write(conn, buf, size);

    if (r == -1)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
      {
        return false;
      }
      close();
      return false;
    }

    return true;
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

  auto inet_socket::make_tcp(const char *h, const int p) -> inet_socket *
  {
    if (!h || p <= 0 || p > 65535)
    {
      return nullptr;
    }
    return new inet_socket(h, p, address_support::ip4, type_support::tcp);
  }

  auto inet_socket::make_tcp6(const char *h, const int p) -> inet_socket *
  {
    if (!h || p <= 0 || p > 65535)
    {
      return nullptr;
    }
    return new inet_socket(h, p, address_support::ip6, type_support::tcp);
  }

  auto inet_socket::make_udp(const char *h, const int p) -> inet_socket *
  {
    if (!h || p <= 0 || p > 65535)
    {
      return nullptr;
    }
    return new inet_socket(h, p, address_support::ip4, type_support::udp);
  }

  auto inet_socket::make_udp6(const char *h, const int p) -> inet_socket *
  {
    if (!h || p <= 0 || p > 65535)
    {
      return nullptr;
    }
    return new inet_socket(h, p, address_support::ip6, type_support::udp);
  }

  // ---------------------------------------------------------------------------
  // PRIVATE FUNCTIONS
  // ---------------------------------------------------------------------------
  auto inet_socket::update_addr_host(const char *h) -> void
  {
    int result = inet_pton(domain_, h, &(inet_addr.sin_addr));
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
    inet_addr.sin_port = htons(static_cast<uint16_t>(p));
  }
} // namespace io
