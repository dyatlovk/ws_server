#pragma once

#include <http/stream.h++>

#undef TEST_EXIT_ONFAIL
#define TEST_EXIT_ONFAIL 1
#include "../testSuite.h"

namespace tests::http::stream
{
  TEST_CASE(content, {
    ::http::stream stream;
    stream.write_bytes({'b', 'o', 'd', 'y'});
    ASSERT_EQ_CHAR(stream.get_contents(), "body", "content ok");
    ASSERT_EQ_INT(stream.get_size(), 4, "size ok");
  });

  TEST_CASE(reset, {
    ::http::stream stream;
    stream.write_bytes({'b', 'o', 'd', 'y'});
    ASSERT_EQ_CHAR(stream.get_contents(), "body", "content ok");
    stream.reset();
    ASSERT_EQ_CHAR(stream.get_contents(), "", "reset ok");
    stream.write_bytes({'t'});
    ASSERT_EQ_CHAR(stream.get_contents(), "t", "rewrite ok");
  });

  auto run() -> void
  {
    content();
    reset();
  }
} // namespace tests::http::stream
