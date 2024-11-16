#include <functional>
#include <http/middlewares/response.h++>
#include <http/options.h++>
#include <http/router.h++>
#include <http/server.h++>

#include "Controllers/Blog.h++"
#include "Controllers/Default.h++"

using router = http::router;
using Blog = Controllers::Blog;
using Default = Controllers::Default;
using method = http::request::methods;

#define CONTROLLER(method, instance) std::bind(&method, &instance, std::placeholders::_1, std::placeholders::_2)

int main(int argc, char *argv[])
{
  auto options = http::options({3044, "127.0.0.1", "Black Mesa", "/public"});
  http::server app(&options);
  http::middlewares::response response_middleware(&options);
  app.add_middleware(&response_middleware);

  router router;

  Default def;
  router.add("/", method::Get, CONTROLLER(Default::index, def));
  router.add("/about", method::Get, CONTROLLER(Default::about, def));
  router.add("/about/\\d+", method::Get, CONTROLLER(Default::about, def));
  router.add("/faq", method::Get, CONTROLLER(Default::faq, def));

  Blog blog;
  router.add("/blog", method::Get, CONTROLLER(Blog::list, blog));
  router.add("/blog/\\d+", method::Get, CONTROLLER(Blog::entry, blog));

  app.with_routers(&router);

  const auto exit_code = app.listen();
  router.shutdown();
  return exit_code;
}
