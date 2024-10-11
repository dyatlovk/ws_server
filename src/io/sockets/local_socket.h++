#pragma once

#include <sys/un.h>

#include "socket.h++"

namespace io
{
  class local_socket : public io::socket
  {
  public:
    local_socket(const char *path);

    ~local_socket() override;

    auto open() -> bool override;

    auto bind() -> bool override;

    auto listen(const int max = 1000) -> bool override;

    auto close() -> void override;

    auto accept() -> int override;

    auto read(const int conn, const int bufSize = 1024) -> const std::string & override;

    auto write(const int conn, const char *buf, int size) -> bool override;

    auto write_chunked(const int conn, const char *buf, int size) -> int override;

    auto set_non_blocking() -> void;

  private:
    // path to socket
    const char *path_;

    struct sockaddr_un unix_addr;
  };
} // namespace io
