#include <http/server.h++>

#include "http/middlewares/response.h++"
#include "options.h++"

using request = http::request;
using router = http::router;
using response = http::response;
using method = request::methods;

int main()
{
  const auto options = server::get_options();
  http::server app(&options);
  http::middlewares::response response_middleware;
  app.add_middleware(&response_middleware);

  router router;
  router.add("/", method::Get, [](const request *req, response *res) { res->with_body("index"); });
  app.with_routers(&router);
  router.shutdown();

  const auto exit_code = app.listen();
  return exit_code;
}
