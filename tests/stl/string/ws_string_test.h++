#pragma once

#include "stl/string/ws_string.h++"
#define TEST_EXIT_ONFAIL 1
#include "../../testSuite.h"

namespace tests::stl
{
  TEST_CASE(trim, {
    auto lead_trim = ws_stl::trim_string("  key");
    ASSERT_EQ_CHAR("key", lead_trim.c_str(), "lead trim ok");

    auto end_trim = ws_stl::trim_string("key  ");
    ASSERT_EQ_CHAR("key", end_trim.c_str(), "end trim ok");
  });

  TEST_CASE(split, {
    auto result = ws_stl::split_string("key:value", ':');
    ASSERT_EQ_CHAR("key", result.begin()->first.c_str(), "key ok");
    ASSERT_EQ_CHAR("value", result.begin()->second.c_str(), "value ok");

    auto r2 = ws_stl::split_string("key :value", ':');
    ASSERT_EQ_CHAR("key", result.begin()->first.c_str(), "key1 ok");
    ASSERT_EQ_CHAR("value", result.begin()->second.c_str(), "value1 ok");

    auto r3 = ws_stl::split_string("key  :  value", ':');
    ASSERT_EQ_CHAR("key", result.begin()->first.c_str(), "key2 ok");
    ASSERT_EQ_CHAR("value", result.begin()->second.c_str(), "value3 ok");
  })

  TEST_CASE(uppercase, {
    auto upper = ws_stl::to_upper("lowercase");
    ASSERT_EQ_CHAR("LOWERCASE", upper.c_str(), "uppercase 1");
    auto u2 = ws_stl::to_upper("LoWerCASE");
    ASSERT_EQ_CHAR("LOWERCASE", u2.c_str(), "lowercase 2");
  })

  TEST_CASE(lowercase, {
    auto lower = ws_stl::to_lower("UPPERCASE");
    ASSERT_EQ_CHAR("uppercase", lower.c_str(), "lowercase 1");
    auto l2 = ws_stl::to_lower("UpperCASE");
    ASSERT_EQ_CHAR("uppercase", l2.c_str(), "lowercase 2");
  })

  auto run() -> void
  {
    split();
    trim();
    uppercase();
    lowercase();
  }
} // namespace tests::stl
