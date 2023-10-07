#pragma once

#include <io/sockets/local_socket.h++>
#include <unistd.h>

#include "../testSuite.h"

namespace tests::socket_local
{
  typedef ::io::local_socket sock;

  const char *path = "/var/tmp/test.socket";

  auto make_unix() -> sock *
  {
    auto s = new sock(path);
    s->open();
    s->bind();
    s->listen();
    return s;
  }

  auto is_file_exist(const char *path) -> bool
  {
    return access(path, R_OK) != -1;
  }

  TEST_CASE(socket_unix, {
    auto s = make_unix();
    const int id = s->get_fd();
    ASSERT_TRUE(s->get_fd() > 0, "socket unix");
    ASSERT_TRUE(is_file_exist(path), "socket exist");
    delete s;

    ASSERT_FALSE(is_file_exist(path), "socket removed on exit");
  })

  TEST_CASE(socket_statuses, {
    auto s = new sock(path);
    s->open();
    ASSERT_TRUE(s->get_state() == ::io::socket::states::opened, "socket opened");

    s->bind();
    ASSERT_TRUE(s->get_state() == ::io::socket::states::binded, "socket binded");

    s->listen();
    ASSERT_TRUE(s->get_state() == ::io::socket::states::listen, "socket listened");

    delete s;
  });

  TEST_CASE(socket_flags, {
    auto s = make_unix();

    ASSERT_FALSE(s->get_flags() & O_NONBLOCK, "socekt not contain O_NONBLOCK flag");

    s->append_flags(O_NONBLOCK);
    ASSERT_TRUE(s->get_flags() & O_NONBLOCK, "socket contain O_NONBLOCK flag");

    s->set_flags(O_ASYNC);
    ASSERT_FALSE(s->get_flags() & O_NONBLOCK, "socket not contain O_NONBLOCK flag");
    ASSERT_TRUE(s->get_flags() & O_ASYNC, "socket contain O_ASYNC flag");

    delete s;
  });

  auto run() -> void
  {
    socket_unix();
    socket_statuses();
    socket_flags();
  }
} // namespace tests::socket_local
