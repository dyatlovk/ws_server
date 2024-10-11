#pragma once

#include <io/sockets/inet_socket.h++>

#define TEST_EXIT_ONFAIL 1
#include "../testSuite.h"

namespace tests::socket_inet
{
  typedef ::io::inet_socket sock;

  auto make_tcp() -> sock *
  {
    auto s = new sock("127.0.0.1", 3030, sock::address_support::ip4, sock::type_support::tcp);
    s->open();
    s->reuse_addr(false);
    s->reuse_port(false);
    s->set_non_blocking();
    s->bind();
    s->listen();
    return s;
  }

  auto make_udp() -> sock *
  {
    auto s = new sock("127.0.0.1", 3030, sock::address_support::ip4, sock::type_support::udp);
    s->open();
    s->reuse_addr(false);
    s->reuse_port(false);
    s->set_non_blocking();
    s->bind();
    s->listen();
    return s;
  }

  auto send_packet(const char *data) -> void
  {
    std::string s;
    s.append("echo ").append(data).append(" | socat tcp:localhost:3030 -");
    system(s.c_str());
  }

  TEST_CASE(socket_tcp, {
    auto s = make_tcp();
    const int id = s->get_fd();
    ASSERT_TRUE(s->get_fd() > 0, "tcp ip4");

    s->open();
    s->bind();
    s->listen();

    ASSERT_TRUE(s->get_fd() == id, "one socket per instance");
    delete s;
  });

  TEST_CASE(socket_udp, {
    auto s = make_udp();
    const int id = s->get_fd();
    ASSERT_TRUE(s->get_fd() > 0, "udp");

    s->open();
    s->bind();
    s->listen();
    ASSERT_TRUE(s->get_fd() == id, "one socket per instance");

    delete s;
  });

  TEST_CASE(socket_statuses, {
    auto s = new sock("127.0.0.1", 3030, sock::address_support::ip4, sock::type_support::tcp);

    s->open();
    ASSERT_TRUE(s->get_state() == ::io::socket::states::opened, "opened");

    s->bind();
    ASSERT_TRUE(s->get_state() == ::io::socket::states::binded, "binded");

    s->listen();
    ASSERT_TRUE(s->get_state() == ::io::socket::states::listen, "listened");

    delete s;
  });

  TEST_CASE(socket_flags, {
    auto s = make_tcp();

    { // get flag
      s->set_flags(0);
      ASSERT_FALSE(s->get_flags() & O_ASYNC, "get flag");
    }

    { // append flag
      s->set_flags(0);
      s->append_flags(O_NONBLOCK);
      s->append_flags(O_ASYNC);
      ASSERT_TRUE(s->get_flags() & O_NONBLOCK | O_ASYNC, "append a flag");
    }

    { // remove flag
      s->set_flags(0);
      s->append_flags(O_ASYNC);
      s->rm_flag(O_ASYNC);
      ASSERT_FALSE(s->get_flags() & O_ASYNC, "rm flag");
    }

    { // set flags
      s->set_flags(0);
      s->set_flags(O_ASYNC | O_APPEND);
      ASSERT_TRUE(s->get_flags() & O_ASYNC | O_APPEND, "set flag");
    }

    delete s;
  });

  TEST_CASE(socket_read, {
    auto server = make_tcp();
    send_packet("hello");
    const auto connection = server->accept();
    const auto data = server->read(connection);

    ASSERT_TRUE(connection > 0, "new connection");
    ASSERT_EQ_CHAR(data.c_str(), "hello\n", "read data");

    delete server;
  })

  TEST_CASE(socket_write, {
    auto s = make_tcp();
    send_packet("hello");
    const auto connection = s->accept();
    auto is_sended = s->write(connection, "test", 4);
    ASSERT_TRUE(is_sended, "send data");
    delete s;
  });

  TEST_CASE(socket_opt, {
    auto s = make_tcp();
    auto is_set = s->set_opt(SOL_SOCKET, SO_REUSEPORT, 0);
    ASSERT_TRUE(is_set, "set option");

    auto res = s->get_opt(SOL_SOCKET, SO_REUSEPORT);
    ASSERT_EQ_INT(res, 0, "r/w option");

    delete s;
  })

  auto run() -> void
  {
    socket_tcp();
    socket_udp();
    socket_statuses();
    socket_flags();
    socket_write();
    socket_read();
    socket_opt();
  }
} // namespace tests::socket_inet
