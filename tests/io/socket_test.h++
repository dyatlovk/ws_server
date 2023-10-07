#pragma once

#include <fcntl.h>
#include <io/sockets.h++>
#include <unistd.h>

#include "../testSuite.h"

namespace tests::io
{
  typedef ::io::sockets io;
  typedef io::inet_proto_family proto_family;
  typedef io::inet_type inet_type;

  auto make_socket_inet() -> ::io::sockets *
  {
    auto s = new ::io::sockets();
    s->open_inet("127.0.0.1", 3030, proto_family::ip4, inet_type::tcp);
    s->bind();
    s->listen();

    return s;
  }

  auto is_file_exist(const char *path) -> bool
  {
    return access(path, R_OK) != -1;
  }

  auto free_socket(::io::sockets *sock) -> void
  {
    delete sock;
  }

  TEST_CASE(socket_create_inet, {
    auto tcp = new ::io::sockets();
    tcp->open_inet("127.0.0.1", 3030, proto_family::ip4, inet_type::tcp);
    tcp->bind();
    tcp->listen();
    ASSERT_TRUE(tcp->get_fd() > 0, "socket tcp opened");
    free_socket(tcp);

    auto udp = new ::io::sockets();
    udp->open_inet("127.0.0.1", 3030, proto_family::ip4, inet_type::udp);
    udp->bind();
    udp->listen();
    ASSERT_TRUE(udp->get_fd() > 0, "socket udp opened");
    free_socket(udp);
  });

  TEST_CASE(socket_create_unix, {
    const char *path = "/var/tmp/test.socket";
    auto un = new ::io::sockets();
    un->open_unix(path);
    un->bind();
    un->listen();
    ASSERT_TRUE(un->get_fd() > 0, "socket unix opened");
    ASSERT_TRUE(is_file_exist(path), "socket exist");
    free_socket(un);
    ASSERT_FALSE(is_file_exist(path), "socket removed on exit");
  });

  TEST_CASE(single_socket, {
    auto s = make_socket_inet();
    const int sock = s->get_fd();

    // trying opening a new socket at the same instance
    s->open_inet("127.0.0.1", 3030, proto_family::ip4, inet_type::tcp);
    s->bind();
    s->listen();
    ASSERT_TRUE(s->get_fd() == sock, "one socket per instance");

    free_socket(s);
  });

  TEST_CASE(socket_statuses, {
    auto s = new ::io::sockets();
    s->open_inet("127.0.0.1", 3030, proto_family::ip4, inet_type::tcp);
    ASSERT_TRUE(s->get_status() == ::io::sockets::status::opened, "socket opened");

    s->bind();
    ASSERT_TRUE(s->get_status() == ::io::sockets::status::binded, "socket binded");

    s->listen();
    ASSERT_TRUE(s->get_status() == ::io::sockets::status::listen, "socket listened");

    free_socket(s);
  });

  TEST_CASE(socket_flags, {
    auto s = make_socket_inet();
    ASSERT_FALSE(s->get_flags() & O_NONBLOCK, "socekt not contain O_NONBLOCK flag");

    s->append_flags(O_NONBLOCK);
    ASSERT_TRUE(s->get_flags() & O_NONBLOCK, "socket contain O_NONBLOCK flag");

    s->set_flags(O_ASYNC);
    ASSERT_FALSE(s->get_flags() & O_NONBLOCK, "socket not contain O_NONBLOCK flag");
    ASSERT_TRUE(s->get_flags() & O_ASYNC, "socket contain O_ASYNC flag");

    free_socket(s);
  });
} // namespace tests::io
