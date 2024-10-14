#pragma once

#include <functional>
#include <http/request.h++>
#include <http/response.h++>
#include <io/epoll/epoll.h++>
#include <io/sockets/inet_socket.h++>
#include <utils/thread_pool.h++>
#include <vector>

namespace examples
{
  class server
  {
    static constexpr int PORT_DEFAULT = 3044;
    static constexpr const char *CRLF = "\r\n";
    static constexpr const char *HOST_DEFAULT = "127.0.0.1";

    struct router;

    using req = http::request;
    using router_func = std::function<http::response(http::request *)>;
    using router_map = std::vector<router *>;

  public:
    server(const char *host = HOST_DEFAULT, int port = PORT_DEFAULT);

    ~server();

    auto run() -> void;

    auto add_route(const char *url, req::methods method, router_func &&handler) -> void;

    auto serve_static(const char *dir) -> void { this->static_dir_ = dir; }

    auto shutdown() -> void;

  private:
    int port_;
    const char *host_;

    io::epoll *epoll_;
    io::inet_socket *srv_;

    std::string static_dir_;
    utils::thread_pool thr_pool;

  private:
    struct router
    {
      const char *url;
      req::methods method;
      router_func handler;
    };
    std::vector<router *> routes_;
    auto match_route(const char *url) -> router *;

  private:
    auto read_tpl(const std::string &p) -> const std::string;

    auto from_char(const unsigned char *data) -> std::vector<char>;

    auto list_served_files() -> void;

    unsigned char favicon_png[656] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49,
        0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3,
        0xff, 0x61, 0x00, 0x00, 0x02, 0x57, 0x49, 0x44, 0x41, 0x54, 0x38, 0x4f, 0x55, 0x53, 0xdb, 0x6a, 0x53, 0x41,
        0x14, 0x5d, 0x73, 0x4e, 0x6e, 0x0d, 0x9c, 0x18, 0x51, 0x34, 0x5a, 0x29, 0x2a, 0xa5, 0x9a, 0xb6, 0x20, 0xf4,
        0x0b, 0x7c, 0x29, 0x22, 0x78, 0x41, 0x6c, 0xed, 0x27, 0xf4, 0x41, 0xf0, 0x03, 0xac, 0x7d, 0x90, 0xd2, 0x0f,
        0xf0, 0x33, 0x92, 0x48, 0x1f, 0x54, 0x7c, 0xf0, 0x31, 0x6f, 0x62, 0x6d, 0x41, 0xf0, 0xf2, 0xa0, 0x20, 0x26,
        0xd2, 0x62, 0x48, 0x8d, 0x44, 0x4a, 0x2e, 0xce, 0x8c, 0x6b, 0xcf, 0x9c, 0xd3, 0xa4, 0x13, 0xc8, 0xcc, 0xd9,
        0x7b, 0xcd, 0x5e, 0x7b, 0xf6, 0x5e, 0x5b, 0xe9, 0xa5, 0xca, 0x7a, 0x0a, 0x6a, 0xcd, 0x84, 0x26, 0x0b, 0x13,
        0xe0, 0xd8, 0xb2, 0xfc, 0x52, 0xde, 0xa2, 0xe9, 0x0a, 0x0c, 0x20, 0xa6, 0x80, 0xc6, 0xd0, 0xda, 0xbe, 0x86,
        0xdd, 0x54, 0x76, 0xa9, 0xda, 0xe3, 0x77, 0xd6, 0x04, 0x44, 0x1a, 0x4b, 0x27, 0x17, 0x51, 0x56, 0x29, 0x9a,
        0x05, 0x9e, 0x2c, 0xde, 0xe6, 0x32, 0x44, 0x1c, 0xd1, 0x68, 0xf4, 0x95, 0x59, 0xae, 0x5a, 0x4f, 0x22, 0x00,
        0xef, 0x72, 0xc4, 0x12, 0x84, 0x36, 0x15, 0xd2, 0x26, 0x2e, 0x67, 0x1c, 0x61, 0x1c, 0x8e, 0x36, 0xc9, 0xc0,
        0xfa, 0x34, 0x8f, 0x3b, 0xdd, 0x97, 0x5c, 0x0a, 0x63, 0x97, 0x03, 0x79, 0x8c, 0x61, 0x34, 0xa1, 0x62, 0x78,
        0x06, 0x60, 0x06, 0xc9, 0x55, 0x29, 0x41, 0x60, 0x04, 0x68, 0xf9, 0x93, 0x07, 0xc4, 0x4f, 0x72, 0x79, 0x79,
        0xbb, 0xdb, 0xad, 0x86, 0x56, 0x81, 0x8b, 0x7d, 0x94, 0x81, 0x66, 0x3e, 0x1f, 0x3a, 0x1d, 0x5c, 0x2d, 0x14,
        0x70, 0x30, 0x1c, 0xc2, 0x30, 0xea, 0xd9, 0x5c, 0x06, 0x9f, 0xfe, 0x74, 0x30, 0x1d, 0x9d, 0xc0, 0xb7, 0xbf,
        0x5d, 0x18, 0xbe, 0xab, 0x98, 0x0e, 0x71, 0x29, 0x5f, 0x88, 0x0b, 0xe3, 0x32, 0xa8, 0x90, 0x86, 0x6c, 0x2c,
        0xe0, 0xc5, 0x97, 0xaf, 0xf1, 0x6c, 0xe1, 0x1a, 0xb6, 0x9a, 0x3f, 0xb1, 0xd7, 0xeb, 0x61, 0x63, 0x7e, 0x1e,
        0x8b, 0xf5, 0x3a, 0xee, 0x4f, 0x4e, 0xe2, 0xdd, 0xc1, 0x6f, 0x5c, 0x29, 0x44, 0x58, 0x28, 0x16, 0xf1, 0x64,
        0x76, 0xd6, 0x07, 0x60, 0xc6, 0xca, 0xae, 0xd4, 0x98, 0x97, 0xe6, 0x83, 0x02, 0xac, 0x6e, 0x6f, 0x23, 0xca,
        0xa4, 0x51, 0xfb, 0xd1, 0xc0, 0x80, 0x19, 0x3c, 0x9c, 0xbe, 0x8c, 0xb7, 0xed, 0x36, 0xd2, 0x2c, 0xe4, 0x3f,
        0x12, 0x5c, 0x3f, 0x75, 0x1a, 0x33, 0x51, 0x11, 0xb7, 0xa6, 0x4a, 0xee, 0x0a, 0x42, 0x56, 0x61, 0x54, 0x44,
        0xa0, 0xda, 0x68, 0xe0, 0xd1, 0xce, 0x2e, 0x66, 0xc8, 0x54, 0xca, 0xe5, 0x50, 0x6f, 0xb5, 0xf0, 0xb8, 0x5c,
        0xc6, 0xcd, 0x73, 0x25, 0x3c, 0x6f, 0x34, 0xd1, 0xd3, 0x1a, 0x1b, 0x5f, 0x3e, 0xa3, 0x73, 0xef, 0x2e, 0xa2,
        0x30, 0xe3, 0xaa, 0xe2, 0xda, 0xe8, 0xea, 0xc9, 0xff, 0xce, 0x60, 0x88, 0x93, 0x2f, 0x5e, 0xe1, 0xe9, 0xdc,
        0x1c, 0xce, 0xf3, 0xfd, 0xab, 0xef, 0x77, 0xf1, 0xf1, 0xc6, 0x22, 0xb6, 0xf6, 0x9a, 0xd8, 0x69, 0x77, 0x5c,
        0x0f, 0xde, 0xec, 0xff, 0x42, 0xeb, 0xce, 0x6d, 0x64, 0x53, 0x21, 0x02, 0x79, 0x82, 0x59, 0xae, 0x59, 0x25,
        0xfd, 0xf2, 0x3a, 0xc1, 0xd7, 0x6e, 0x17, 0x67, 0xc8, 0x9e, 0xa2, 0x90, 0xbe, 0x1f, 0x1e, 0xa2, 0x1c, 0x45,
        0x68, 0xf5, 0xfb, 0xd8, 0x1f, 0xf4, 0xd0, 0x1d, 0x6a, 0x4c, 0x4d, 0x4c, 0xe0, 0x42, 0x3e, 0x2f, 0x0d, 0x24,
        0x5a, 0x93, 0xf8, 0x41, 0xd5, 0x5a, 0x4a, 0xc9, 0x05, 0x19, 0x6f, 0x57, 0x1c, 0x30, 0xe9, 0x5c, 0xec, 0x1c,
        0xdb, 0x7c, 0xf3, 0x9d, 0x0e, 0x12, 0xab, 0x1c, 0xbc, 0xd6, 0xe3, 0xc5, 0x83, 0xa5, 0x2e, 0xbc, 0x1e, 0x0c,
        0x34, 0x9d, 0x21, 0xfb, 0x2f, 0x20, 0x91, 0x8b, 0xe0, 0x94, 0x59, 0xa9, 0x58, 0xe5, 0xc4, 0x93, 0x08, 0x65,
        0x44, 0x92, 0x24, 0x91, 0x04, 0x34, 0xf2, 0xce, 0x80, 0xf2, 0x91, 0xcd, 0x51, 0x89, 0xdc, 0x38, 0x4c, 0x1c,
        0x9c, 0xac, 0xbf, 0x96, 0x8c, 0xdf, 0xf8, 0x2e, 0x29, 0xf1, 0x86, 0x8c, 0x63, 0x2c, 0xeb, 0x64, 0x56, 0x38,
        0x7c, 0x7d, 0xb6, 0xb1, 0xb2, 0x4e, 0xf6, 0x35, 0xc2, 0xb2, 0x32, 0x55, 0x6e, 0x2c, 0x92, 0x9a, 0xc6, 0x69,
        0x8a, 0xe6, 0x3d, 0x63, 0xbc, 0xfc, 0xb1, 0x4f, 0xe0, 0xe6, 0x7f, 0x34, 0xa2, 0x00, 0xd2, 0xb5, 0xcd, 0x7d,
        0x98, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};
  };
} // namespace examples
