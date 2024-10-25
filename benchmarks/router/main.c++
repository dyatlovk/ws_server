#include <chrono>
#include <http/router.h++>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int main()
{
  const int requests = 1000;
  using request = ::http::request;
  using response = ::http::response;
  using method = request::methods;

  ::http::router router;

  std::vector<std::string> urls = {"/catalog/women/item/34", "/", "/articles/entry", "/not-found", "/blog/regex"};
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(urls.begin(), urls.end(), g);

  router.add("/", method::Get,
      [](request *req, response *res) {

      });

  router.add("/articles/entry", {method::Get, method::Patch},
      [](request *req, response *res) {

      });

  router.add("/blog/\\D+", method::Get,
      [](request *req, response *res) {

      });

  router.add("/catalog/\\D+/item/\\d+", method::Get,
      [](request *req, response *res) {

      });

  for (std::string &url : urls)
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < requests; i++)
    {
      router.match(url.c_str());
    }
    auto stop = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(stop - start).count();
    std::cout << elapsed_time_ms / requests << " ms/req \t " << url << std::endl;
  }

  std::cout << "===============" << std::endl;
  std::cout << "Total requests: " << requests * urls.size() << std::endl;

  router.shutdown();
}
