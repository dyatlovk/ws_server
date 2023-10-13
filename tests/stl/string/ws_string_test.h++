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

  auto run() -> void
  {
    split();
    trim();
  }
} // namespace tests::stl
