#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <http/response.h++>
#include <iostream>

const int requests = 100000;
using response = ::http::response;

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

auto main() -> int
{
  std::cout << "MEM: " << getValue() << " Kb" << std::endl;

  const char *phrase = "Benchmark";
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < requests; ++i)
  {
    http::response res{200, "OK"};
    res.with_added_header("Server", phrase);
    res.with_added_header("Content-Type", "text/html;charset=utf-8");
    res.with_body({'t', 'e', 's', 't'});
    auto msg = res.get_message();
  }
  auto stop = std::chrono::high_resolution_clock::now();

  double elapsed_micro = std::chrono::duration<double, std::micro>(stop - start).count();
  std::stringstream ss;
  ss << elapsed_micro / requests << " μs/res ";
  ss << " [" << getValue() << " Kb]";

  std::cout << ss.rdbuf() << std::endl;

  std::cout << std::endl;
  std::cout << "TOTAL" << std::endl;
  std::cout << std::endl;
  std::cout << "RES: " << requests << std::endl;
  std::cout << "MEM: " << getValue() << " Kb" << std::endl;

  return 0;
}
