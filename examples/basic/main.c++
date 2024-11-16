#include <http/options.h++>
#include <http/server.h++>

#include "http/middlewares/response.h++"

using request = http::request;
using router = http::router;
using response = http::response;
using method = request::methods;

int main()
{
  auto options = http::options({3044, "127.0.0.1", "Black Mesa", "/public"});
  http::server app(&options);
  http::middlewares::response response_middleware(&options);
  app.add_middleware(&response_middleware);

  router router;
  router.add("/", method::Get, [](const request *req, response *res) { res->with_body("index"); });
  app.with_routers(&router);
  const auto exit_code = app.listen();
  router.shutdown();
  return exit_code;
}
