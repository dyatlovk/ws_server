#include "TcpAbstract.h++"

#include <unistd.h>

namespace Layers
{
  TcpAbstract::TcpAbstract(const AcceptType type)
      : acceptType(type)
      , max_conn(MAX_CONN)
  {
  }

  TcpAbstract::~TcpAbstract()
  {
    CloseConnection();
  }

  auto TcpAbstract::CloseConnection() -> void
  {
    close(fd);
  }
} // namespace Layers
