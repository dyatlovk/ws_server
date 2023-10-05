#pragma once

#include "layers/tcp.h++"
#include "testSuite.h"

namespace TcpTest
{
  TEST_CASE(Init, {
    auto connection = new Layers::Tcp("127.0.0.1", 3044, 5);
    const auto socket = connection->GetDescriptor()->fd;
    connection->Close(socket);
    auto state = Layers::Tcp::State::Close == *connection->GetState();
    ASSERT_TRUE(state, "Init ok");
    delete connection;
  })
}
