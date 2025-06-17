#include "server.h++"

#include <csignal>
#include <utils/logger.h++>

#include "http/middlewares/response.h++"
#include "http/parser.h++"

namespace http
{
  server *server::instance = nullptr;

  server::server(options_interface *options)
      : srv_(sock::make_tcp(options->get_host(), options->get_port()))
      , options_(options)
      , epoll_(new io::epoll())
      , router_()
      , running_(false)
  {
    this->instance = this;

    srv_->open();
    srv_->set_non_blocking();
    srv_->reuse_addr(true);
    srv_->reuse_port(true);
    srv_->bind();
    srv_->listen();

    try
    {
      epoll_->create();
      epoll_->set_timeout(1000);
      epoll_->register_master(srv_->get_fd(), io::epoll::Events::READ);
    }
    catch (std::runtime_error &e)
    {
      LOG_ERROR("Failed to initialize epoll: {}", e.what());
      shutdown();
      return;
    }

    epoll_->on_connection([this](int socket) {});

    epoll_->on_close([this](int socket) {});

    epoll_->on_write(
        [this](int socket, const char *buf)
        {
          http::parser parser{buf};
          auto req_line = parser.get_req_line();
          http::request req;
          auto request = req.parse(req_line);
          http::response res{200, "OK"};
          res.with_added_header("Server", options_->get_name());
          for (auto &mid : middlewares_)
          {
            const auto m = static_cast<http::middlewares::response *>(mid);
            m->set_router(&router_);
            m->execute(&req, res);
          }
          auto msg = res.get_message();
          srv_->write(socket, msg, std::strlen(msg));
          epoll_->unwatch(socket);
          router_.reset();
        });

    running_ = true;
  }

  server::~server()
  {
    if (epoll_) delete epoll_;
    // srv_ is now a smart pointer - no manual delete needed
    LOG_INFO("Server is shutting down");
  }

  auto server::listen() -> int
  {
    // Catch user CTRL+C
    std::signal(SIGINT, &server::signal_handler);

    while (running_)
    {
      try
      {
        epoll_->wait();
      }
      catch (std::runtime_error &e)
      {
        LOG_ERROR("Exception during epoll wait: {}", e.what());
      }
    }

    return 0;
  }

  auto server::shutdown() -> void
  {
    running_ = false;
  }
} // namespace http
