#pragma once
#include <cstdint>
#include <sys/epoll.h>

#include "io/epoll/events.h++"

namespace io
{
  class Epoll
  {
  private:
    constexpr static const unsigned int MAX_EVENTS = 1000;

  public:
    explicit Epoll(const int max = MAX_EVENTS);

    ~Epoll();

    auto Create() -> void;

    auto Wait() -> void;

    auto Close() -> void;

    /**
     * Add socket to monitoring by epoll
     * @param e epoll events(man epoll_ctl)
     **/
    auto RegisterSocket(const int socket, const ::Epoll::Events &e = ::Epoll::Events::INCOMING) -> void;

    /**
     * Remove socket from epoll monitoring
     **/
    auto UnregisterSocket(const int socket) -> void;

  private:
    int fd; // epoll file descriptor
    struct epoll_event *evlist, events[MAX_EVENTS];
    int wait_events;
    int max;
  };
} // namespace io
