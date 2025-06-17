#pragma once

#include <arpa/inet.h>
#include <memory>
#include <string>

#include "socket.h++"

namespace io
{
  class inet_socket : public io::socket
  {
  public:
    enum class address_support : uint8_t
    {
      ip4 = AF_INET,
      ip6 = AF_INET6,
    };

    enum class type_support : uint8_t
    {
      tcp = SOCK_STREAM,
      udp = SOCK_DGRAM,
      icmp = SOCK_RAW,
    };

    inet_socket(const char *host, const int port, address_support address, type_support type);

    ~inet_socket() override;

    auto open() -> bool override;

    auto bind() -> bool override;

    auto listen(const int max = 1000) -> bool override;

    auto close() -> void override;

    auto accept() -> int override;

    auto read(const int conn, const int bufSize = 1024) -> const std::string & override;

    auto write(const int conn, const char *buf, int size) -> bool override;

    auto write_chunked(const int conn, const char *buf, const int chunk) -> int override;

    auto set_non_blocking() -> void;

    // Aliases for tcp
    static auto make_tcp(const char *h, const int p) -> std::unique_ptr<inet_socket>;
    static auto make_tcp6(const char *h, const int p) -> std::unique_ptr<inet_socket>;

    // aliases for udp
    static auto make_udp(const char *h, const int p) -> std::unique_ptr<inet_socket>;
    static auto make_udp6(const char *h, const int p) -> std::unique_ptr<inet_socket>;

  private:
    auto update_addr_host(const char *h) -> void;
    auto update_addr_port(const int p) -> void;

  private:
    int port_;
    std::string host_; // Store copy instead of pointer

    // Union to support both IPv4 and IPv6
    union
    {
      struct sockaddr_in inet4_addr;
      struct sockaddr_in6 inet6_addr;
      struct sockaddr generic_addr;
    } addr_;
  };
} // namespace io
