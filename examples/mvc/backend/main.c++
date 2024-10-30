#include <functional>
#include <http/router.h++>

#include "Controllers/Blog.h++"
#include "Controllers/Default.h++"
#include "server.h++"

using router = http::router;
using Blog = Controllers::Blog;
using Default = Controllers::Default;
using method = http::request::methods;

#define CONTROLLER(method, instance) std::bind(&method, &instance, std::placeholders::_1, std::placeholders::_2)

int main(int argc, char *argv[])
{
  ::examples::server server{"127.0.0.1", 3044};
  router router;

  Default def;
  router.add("/",             method::Get, CONTROLLER(Default::index, def));
  router.add("/about",        method::Get, CONTROLLER(Default::about, def));
  router.add("/about/\\d+",   method::Get, CONTROLLER(Default::about, def));
  router.add("/faq",          method::Get, CONTROLLER(Default::faq, def));

  Blog blog;
  router.add("/blog",       method::Get, CONTROLLER(Blog::list, blog));
  router.add("/blog/\\d+",  method::Get, CONTROLLER(Blog::entry, blog));

  server.with_routers(&router);
  server.run();
  router.shutdown();
  server.shutdown();

  return 0;
}
