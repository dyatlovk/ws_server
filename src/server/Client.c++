#include "Client.h++"

namespace Server
{
  Client::Client(int socket)
  {
    fd = socket;
  }

  auto Client::Accept() -> int
  {
    int socket = -1;
    socklen_t peer_len = sizeof(addr);
    if ((socket = accept(fd, (struct sockaddr *)&addr, &peer_len)) == -1)
    {
      return -1;
    }

    getsockname(socket, (struct sockaddr *)&addr, &peer_len);

    const auto ip = inet_ntoa(addr.sin_addr);
    const auto p = (int)ntohs(addr.sin_port);

    return socket;
  }
} // namespace Server
