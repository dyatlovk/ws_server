#include "local_socket.h++"

#include <fmt/core.h>
#include <sys/socket.h>
#include <unistd.h>

namespace io
{
  local_socket::local_socket(const char *path)
      : path_(path)
  {
    state_ = states::noinit;
    fd = -1;
    readBuf.clear();
  }

  local_socket::~local_socket()
  {
    close();
    fmt::println("[Sock] [{}] free", fd);
  }

  auto local_socket::open() -> bool
  {
    domain_ = AF_UNIX;
    type_ = SOCK_SEQPACKET;
    proto_ = 0;

    if (state_ >= states::opened) return true;

    fd = ::socket(domain_, type_, proto_);
    if (fd == -1)
    {
      state_ = states::error;
      return false;
    }

    memset(&unix_addr, 0, sizeof(unix_addr));
    unix_addr.sun_family = domain_;
    strncpy(unix_addr.sun_path, path_, sizeof(unix_addr.sun_path) - 1);

    state_ = states::opened;

    fmt::println("[Sock] [{}(unix)] opened OK", fd);

    return true;
  }

  auto local_socket::bind() -> bool
  {
    if (state_ >= states::binded) return true;

    const int b = ::bind(fd, (const struct sockaddr *)&unix_addr, sizeof(unix_addr));

    if (b == -1)
    {
      state_ = states::error;
      return false;
    }

    state_ = states::binded;

    fmt::println("[Sock] [{}] binded [OK]", fd);

    return true;
  }

  auto local_socket::listen(const int max) -> bool
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

  auto local_socket::close() -> void
  {
    ::close(fd);
    remove(path_);
    state_ = states::closed;
    fmt::println("[Sock] [{}] closed", fd);
  }

  auto local_socket::accept() -> int
  {
    struct sockaddr peer_addr;
    socklen_t peer_len = sizeof(unix_addr);
    const int peer = ::accept(fd, (struct sockaddr *)&peer_addr, &peer_len);

    if (peer == -1)
    {
      state_ = states::error;
      return -1;
    }

    return peer;
  }

  auto local_socket::read(const int conn, const int bufSize) -> const std::string &
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

  auto local_socket::write(const int conn, const char *buf, int size) -> bool
  {
    return true;
  }

  auto local_socket::write_chunked(const int conn, const char *buf, int size) -> int
  {
    return 0;
  }

  auto local_socket::set_non_blocking() -> void
  {
    append_flags(O_NONBLOCK);
  }
} // namespace io
