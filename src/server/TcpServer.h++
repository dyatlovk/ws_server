#pragma once
#include <layers/TcpAbstract.h++>
#include <memory>
#include <unordered_map>

#include "Client.h++"
#include "ServerAbstract.h++"

namespace Server
{
  class TcpServer : public ServerAbstract
  {
  private:
    constexpr static const unsigned int MAX_CONNECTIONS = 1000;

    typedef std::shared_ptr<Client> ClientT;

    // {clientId, clientInstance}
    typedef std::unordered_map<int, ClientT> ClientsPool;

  public:
    TcpServer(const char *host, const int port);

    /**
     * Main loop
     **/
    auto Run() -> void override;

    /**
     * Freeing all resources and exiting app
     **/
    auto ShutDown() -> void override;

  private:
    /**
     * Handler for a new connections and communications with a clients
     **/
    auto WaitConnectionHandler() -> void;

    /**
     * Main blocking loop
     **/
    auto MainLoopHandler() -> void;

  private:
    /**
     * Add new client to pool
     **/
    auto RegisterClient(const int socket) -> void;

    /**
     * Remove client from pool
     **/
    auto DisconnectClient(const int id) -> void;

    /**
     * Handler for a new message from client
     **/
    auto ReadClientMessage(const int id) -> void;

    /**
     * Find client in a pool
     **/
    auto FindClient(const int id) -> ClientT;

    /**
     * Send data to a client
     **/
    auto SendData(ClientT &&client, const char *buf) -> void;

    /**
     * Create client as a server
     **/
    auto CreateServer() -> void;

  private:
    int port;
    const char *host;

    // clients pool
    ClientsPool clients;

    // server as client instance
    Client *server;
  };
} // namespace Server
