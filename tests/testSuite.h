#pragma once

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_GRAY "\x1b[90m"
#define ANSI_COLOR_BRIGHT "\x1b[91m"

inline auto print_char_as_hex(const char *str) -> void
{
  std::stringstream ss;
  for (int i = 0; i < std::strlen(str); ++i)
    ss << std::hex << (int)str[i];
  std::string mystr = ss.str();
  printf("hex: %s\n", mystr.c_str());
}

#define TEST_CASE(name, ...)                                                \
  void name()                                                               \
  {                                                                         \
    printf(ANSI_COLOR_GREEN "=== %s STARTED === " ANSI_COLOR_RESET, #name); \
    printf("\n" ANSI_COLOR_RESET);                                          \
    __VA_ARGS__                                                             \
    printf(ANSI_COLOR_GREEN "=== %s PASSED === " ANSI_COLOR_RESET, #name);  \
    printf("\n" ANSI_COLOR_RESET);                                          \
    printf("\n" ANSI_COLOR_RESET);                                          \
  }

inline void ASSERT_EQ_INT(int actual, int expected, const char *message)
{
  if (expected != actual)
  {
    printf(ANSI_COLOR_RED "Assertion failed: %s\n" ANSI_COLOR_RESET, message);
    printf(ANSI_COLOR_WHITE "actual: %d expected: %d\n" ANSI_COLOR_RESET, actual, expected);
#if defined TEST_EXIT_ONFAIL && TEST_EXIT_ONFAIL == 1
    exit(-1);
#endif
    return;
  }

  printf(ANSI_COLOR_WHITE "Passed: %s\n" ANSI_COLOR_RESET, message);
}

inline void ASSERT_EQ_FLOAT(float actual, float expected, const char *message)
{
  if (expected != actual)
  {
    printf(ANSI_COLOR_RED "Assertion failed: %s\n" ANSI_COLOR_RESET, message);
    printf(ANSI_COLOR_WHITE "actual: %f expected: %f\n" ANSI_COLOR_RESET, actual, expected);
#if defined TEST_EXIT_ONFAIL && TEST_EXIT_ONFAIL == 1
    exit(-1);
#endif
    return;
  }

  printf(ANSI_COLOR_WHITE "Passed: %s\n" ANSI_COLOR_RESET, message);
}

inline void ASSERT_EQ_CHAR(const char *actual, const char *expected, const char *message)
{
  if (strcmp(expected, actual) != 0)
  {
    printf(ANSI_COLOR_RED "Assertion failed: %s\n" ANSI_COLOR_RESET, message);
    printf(ANSI_COLOR_WHITE "actual: \n");
    print_char_as_hex(actual);
    printf(ANSI_COLOR_WHITE "%s (%zub)\n", actual, strlen(actual));
    printf("%s", "----------\n");
    printf(ANSI_COLOR_WHITE "expected: \n");
    print_char_as_hex(expected);
    printf(ANSI_COLOR_WHITE "%s (%zub)\n", expected, strlen(expected));
#if defined TEST_EXIT_ONFAIL && TEST_EXIT_ONFAIL == 1
    exit(-1);
#endif
    return;
  }

  printf(ANSI_COLOR_WHITE "Passed: %s\n" ANSI_COLOR_RESET, message);
}

inline void ASSERT_TRUE(bool actual, const char *message)
{
  if (actual != true)
  {
    printf(ANSI_COLOR_RED "Assertion failed: %s\n" ANSI_COLOR_RESET, message);
    printf(ANSI_COLOR_WHITE "actual: %d\n" ANSI_COLOR_RESET, actual);
#if defined TEST_EXIT_ONFAIL && TEST_EXIT_ONFAIL == 1
    exit(-1);
#endif
    return;
  }

  printf(ANSI_COLOR_WHITE "Passed: %s\n" ANSI_COLOR_RESET, message);
}

inline void ASSERT_FALSE(bool actual, const char *message)
{
  if (actual != false)
  {
    printf(ANSI_COLOR_RED "Assertion failed: %s\n" ANSI_COLOR_RESET, message);
    printf(ANSI_COLOR_WHITE "actual: %d\n" ANSI_COLOR_RESET, actual);
#if defined TEST_EXIT_ONFAIL && TEST_EXIT_ONFAIL == 1
    exit(-1);
#endif
    return;
  }

  printf(ANSI_COLOR_WHITE "Passed: %s\n" ANSI_COLOR_RESET, message);
}

inline void ASSERT_T(bool actual, const char *msg)
{
  if (actual != false)
  {
    printf(ANSI_COLOR_RED "Assertion failed: %s\n" ANSI_COLOR_RESET, msg);
    printf(ANSI_COLOR_WHITE "actual: %d\n" ANSI_COLOR_RESET, actual);
#if defined TEST_EXIT_ONFAIL && TEST_EXIT_ONFAIL == 1
    exit(-1);
#endif
    return;
  }

  printf(ANSI_COLOR_WHITE "Passed: %s\n" ANSI_COLOR_RESET, msg);

  return;
}
