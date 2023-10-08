#pragma once

#include <arpa/inet.h>
#include <atomic>
#include <fcntl.h>
#include <io/epoll/epoll.h++>
#include <unistd.h>

#include "fmt/core.h"

#define TEST_EXIT_ONFAIL 1
#include "../testSuite.h"

namespace tests::epoll
{
  auto create_socket() -> int
  {
    struct sockaddr_in addr;

    int socket = -1;
    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = (int)htons(3044);

    const int b = ::bind(socket, (const struct sockaddr *)&addr, sizeof(addr));
    ::listen(socket, 1000);

    int old_flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, old_flags | O_NONBLOCK);

    fmt::println("create server socket {}", socket);

    return socket;
  }

  auto send_packet(const char *data) -> void
  {
    // echo "hello" | socat tcp:localhost:3030 -
    std::string s;
    s.append("echo ").append(data).append(" | socat tcp:localhost:3044 -");
    system(s.c_str());
  }

  TEST_CASE(epoll_init, {
    auto e = new io::epoll();
    e->create();
    ASSERT_TRUE(e->get_fd() > 0, "epoll init ok");
    e->shutdown();

    delete e;
  });

  TEST_CASE(reg_unreg_sock, {
    auto s = create_socket();
    auto client = create_socket();

    auto e = new io::epoll();
    e->create();
    e->register_master(s);
    e->watch(client);

    ASSERT_EQ_INT(1, e->watched_size(), "register socket ok");

    e->unwatch(client);
    ASSERT_EQ_INT(0, e->watched_size(), "unregister socket ok");

    delete e;
  });

  TEST_CASE(watch, {
    static std::atomic_uint is_run = 1;
    const char *sended;
    auto server = create_socket();

    auto io = new io::epoll();
    io->create();
    io->register_master(server, EPOLLIN);

    io->on_connection(
        [&io](int sock)
        {
          const char *buf = "test";
          int r = ::write(sock, buf, strlen(buf));
        });
    io->on_close([](int sock) {});
    io->on_write(
        [&sended](int sock, const char *data)
        {
          sended = data;
          is_run = 0;
        });

    send_packet("hello");

    while (is_run)
    {
      io->wait();
    }

    ASSERT_EQ_CHAR(sended, "hello\n", "wait ok");

    delete io;
  });

  auto run() -> void
  {
    watch();
    epoll_init();
    reg_unreg_sock();
  }
} // namespace tests::epoll
