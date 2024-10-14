#include <cstring>
#undef TEST_EXIT_ONFAIL
#define TEST_EXIT_ONFAIL 0

#include <stdexcept>

#include "../public/args/parser.h++"
#include "testSuite.h"

TEST_CASE(too_many_args, {
  int argc = utils::args::MAX_INPUT_DEFAUL + 1;
  char *argv[1]{0};
  try
  {
    utils::args commands{argc, argv};
  }
  catch (std::runtime_error &e)
  {
    ASSERT_TRUE(true, "throwing exception ok");
    return;
  }

  ASSERT_TRUE(false, "exception expected");
})

TEST_CASE(input_limit_ok, {
  int argc = 3;
  char *argv[argc];
  argv[0] = const_cast<char *>("filename");
  argv[1] = const_cast<char *>("--command=value1");
  argv[2] = const_cast<char *>("--option=value2");

  utils::args commands{argc, argv};
  ASSERT_TRUE(true, "limit ok");
})

TEST_CASE(find_options, {
  int argc = 3;
  char *argv[argc];
  argv[0] = const_cast<char *>("filename");
  argv[1] = const_cast<char *>("--command=value1");
  argv[2] = const_cast<char *>("--option=value2");

  utils::args commands{argc, argv};
  auto found1 = commands.find_option("--command");
  auto found2 = commands.find_option("--option");
  auto empty = commands.find_option("--not-found");

  ASSERT_EQ_CHAR(found1.c_str(), "value1", "Found command");
  ASSERT_EQ_CHAR(found2.c_str(), "value2", "Found command");
  ASSERT_EQ_CHAR(empty.c_str(), "", "empty");
})

int main(int argc, char *argv[])
{
  too_many_args();
  input_limit_ok();
  find_options();

  return 0;
}
