#include <algorithm>
#include <chrono>
#include <cstring>
#include <http/router.h++>
#include <iostream>
#include <ostream>
#include <random>
#include <string>
#include <utils/thread_pool.h++>
#include <vector>

int parseLine(char *line)
{
  // This assumes that a digit will be found and the line ends in " Kb".
  int i = strlen(line);
  const char *p = line;
  while (*p < '0' || *p > '9')
    p++;
  line[i - 3] = '\0';
  i = atoi(p);
  return i;
}

int getValue()
{ // Note: this value is in KB!
  FILE *file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];

  while (fgets(line, 128, file) != NULL)
  {
    if (strncmp(line, "VmRSS:", 6) == 0)
    {
      result = parseLine(line);
      break;
    }
  }
  fclose(file);
  return result;
}

auto find_longest_url_size(const std::vector<std::string> *items) -> int
{
  std::vector<int> sizes;
  for (const auto &url : *items)
  {
    sizes.push_back(url.size());
  }
  return *max_element(sizes.begin(), sizes.end());
}

int main()
{
  std::cout << "MEM: " << getValue() << " Kb" << std::endl;

  const int requests = 1000;
  using request = ::http::request;
  using response = ::http::response;
  using method = request::methods;

  utils::thread_pool thread_pool;
  ::http::router router;

  std::vector<std::string> urls = {
      "/catalog/women/item/34", "/", "/articles/entry", "/not-found", "/blog/regex", "/1/2/3/4"};
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(urls.begin(), urls.end(), g);

  const auto longest_url = find_longest_url_size(&urls);

  router.add("/", method::Get,
      [](request *req, response *res) {

      });

  router.add("/1/2/3/4", method::Get,
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
    auto ft = thread_pool.enqueue(
        [&router, &url, longest_url]()
        {
          auto start = std::chrono::high_resolution_clock::now();
          for (int i = 0; i < requests; i++)
          {
            router.match(url.c_str());
          }
          auto stop = std::chrono::high_resolution_clock::now();
          double elapsed_time_ms = std::chrono::duration<double, std::milli>(stop - start).count();
          std::stringstream ss;
          ss << url;
          for (int i = 0; i < longest_url + 3 - url.size(); ++i)
          {
            if (longest_url + 3 == i) continue;
            ss << ".";
          }

          ss << requests / elapsed_time_ms << " req/ms";
          ss << " [" << getValue() << " Kb]";

          std::cout << ss.rdbuf() << std::endl;
        });
    ft.wait();
  }

  std::cout << std::endl;
  std::cout << "TOTAL" << std::endl;
  std::cout << std::endl;
  std::cout << "requests: " << requests * urls.size() << std::endl;
  std::cout << "MEM:   " << getValue() << " Kb" << std::endl;

  router.shutdown();
}
