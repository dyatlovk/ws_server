#pragma once

#include <http/parser.h++>

#define TEST_EXIT_ONFAIL 1
#include "../testSuite.h"

namespace tests::http
{
  const char crlf[] = "GET / HTTP/2\r\n"
                      "Host: localhost\r\n"
                      "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/114.0\r\n"
                      "test: value\r\n"
                      "x-key: test\r\n\r\n"
                      "body";

  const char cr[] = "GET / HTTP/2\r"
                    "Host: localhost\r"
                    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/114.0\r"
                    "test: value\r"
                    "x-key: test\r\r"
                    "body";

  const char lf[] = "GET / HTTP/2\n"
                    "Host: localhost\n"
                    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/114.0\n"
                    "test: value\n"
                    "x-key: test\n\n"
                    "body";

  const char invalid[] = "GET / invalid request";

  TEST_CASE(req_validate, {
    {
      const auto parser = new ::http::parser(crlf);
      const auto is_valid = parser->is_buffer_valid();
      ASSERT_TRUE(is_valid, "crlf request is valid");
      delete parser;
    }

    {
      const auto parser = new ::http::parser(cr);
      const auto is_valid = parser->is_buffer_valid();
      ASSERT_FALSE(is_valid, "cr request is not valid");
      delete parser;
    }

    {
      const auto parser = new ::http::parser(lf);
      const auto is_valid = parser->is_buffer_valid();
      ASSERT_FALSE(is_valid, "lf request is not valid");
      delete parser;
    }

    {
      const auto parser = new ::http::parser(invalid);
      const auto is_valid = parser->is_buffer_valid();
      ASSERT_FALSE(is_valid, "request is not valid");
      delete parser;
    }
  });

  TEST_CASE(body, {
    const auto parser = new ::http::parser(crlf);
    auto result_win = parser->find_body();
    ASSERT_EQ_CHAR("body", result_win.c_str(), "body found ok");
    delete parser;
  })

  TEST_CASE(find_request, {
    const auto parser = new ::http::parser(crlf);
    auto result = parser->get_req_line();
    ASSERT_EQ_CHAR("GET / HTTP/2", result.c_str(), "request ok");
    delete parser;
  });

  TEST_CASE(headers, {
    const auto parser = new ::http::parser(crlf);
    auto result = parser->get_headers();
    ASSERT_EQ_INT(result.size(), 4, "headers container size");
    ASSERT_EQ_CHAR("Host: localhost", result.at(0).c_str(), "header ok");
    ASSERT_EQ_CHAR("User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/114.0", result.at(1).c_str(), "header ok");
    ASSERT_EQ_CHAR("test: value", result.at(2).c_str(), "header ok");
    ASSERT_EQ_CHAR("x-key: test", result.at(3).c_str(), "header ok");
    delete parser;
  });

  auto run() -> void
  {
    body();
    req_validate();
    find_request();
    headers();
  }
} // namespace tests::http
