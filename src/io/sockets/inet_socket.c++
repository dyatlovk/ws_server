#include "inet_socket.h++"

#include <cstring>
#include <fmt/core.h>
#include <sys/socket.h>
#include <unistd.h>

namespace io
{
  inet_socket::inet_socket(const char *host, const int port, address_support address, type_support type)
      : host_(host)
      , port_(port)
  {
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
    fmt::println("[Sock] [{}] free", fd);
  }

  auto inet_socket::open() -> bool
  {
    if (state_ >= states::opened) return true;

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

    fmt::println("[Sock] [{}(inet)] opened [OK]", fd);

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

    fmt::println("[Sock] [{}] binded [OK]", fd);

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
    fmt::println("[Sock] [{}] listen [OK]", fd);

    return true;
  }

  auto inet_socket::close() -> void
  {
    ::close(fd);
    state_ = states::closed;
    fmt::println("[Sock] [{}] closed", fd);
  }

  auto inet_socket::accept() -> int
  {
    struct sockaddr peer_addr;
    socklen_t peer_len = sizeof(inet_addr);
    const int peer = ::accept(fd, (struct sockaddr *)&peer_addr, &peer_len);

    if (peer == -1)
    {
      state_ = states::error;
      return -1;
    }

    return peer;
  }

  auto inet_socket::read(const int conn, const int bufSize) -> const std::string &
  {
    readBuf.clear();
    char buf[bufSize];
    memset(buf, 0, sizeof(buf));

    int byte_count = 0;
    // read from socket until EOF
    while ((byte_count = ::read(conn, buf, bufSize) > 0))
    {
      readBuf.append(buf);
    }

    return readBuf;
  }

  auto inet_socket::write(const int conn, const char *buf, int size) -> bool
  {
    int r = ::write(conn, buf, size);

    if (r == -1)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
      {
        return false;
      }
      close();
      fmt::println("error on send data");
      return false;
    }

    fmt::println("[Sock]: writed {} b to fd#{}", r, conn);

    return true;
  }

  auto inet_socket::write_chunked(const int conn, const char *buf, int size) -> int
  {
    int num_sent = 0;
    while (size > 0)
    {
      num_sent = ::write(conn, buf, size);
      if (num_sent == -1) return num_sent;
      buf += num_sent;
      size -= num_sent;
    }

    return num_sent;
  }

  auto inet_socket::set_non_blocking() -> void
  {
    append_flags(O_NONBLOCK);
  }

  auto inet_socket::make_tcp(const char *h, const int p) -> inet_socket *
  {
    return new inet_socket(h, p, address_support::ip4, type_support::tcp);
  }

  auto inet_socket::make_tcp6(const char *h, const int p) -> inet_socket *
  {
    return new inet_socket(h, p, address_support::ip6, type_support::tcp);
  }

  auto inet_socket::make_udp(const char *h, const int p) -> inet_socket *
  {
    return new inet_socket(h, p, address_support::ip4, type_support::udp);
  }

  auto inet_socket::make_udp6(const char *h, const int p) -> inet_socket *
  {
    return new inet_socket(h, p, address_support::ip6, type_support::udp);
  }

  // ---------------------------------------------------------------------------
  // PRIVATE FUNCTIONS
  // ---------------------------------------------------------------------------
  auto inet_socket::update_addr_host(const char *h) -> void
  {
    inet_pton(domain_, h, &(inet_addr.sin_addr));
  }

  auto inet_socket::update_addr_port(const int p) -> void
  {
    inet_addr.sin_port = (int)htons(p);
  }
} // namespace io
