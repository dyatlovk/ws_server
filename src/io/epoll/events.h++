#pragma once

#include <sys/epoll.h>
namespace Epoll
{
  /**
   * @see man epoll_ctl
   **/
  enum Events
  {
    INCOMING = EPOLLIN,
    READ = EPOLLIN,
    WRITE = EPOLLOUT,
    CLOSE = EPOLLRDHUP,
    SHUTDOWN = EPOLLRDHUP,
  };
} // namespace Epoll
