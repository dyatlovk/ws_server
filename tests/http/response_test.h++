#pragma once

#include <cstdio>
#include <cstring>
#include <http/response.h++>

#undef TEST_EXIT_ONFAIL
#define TEST_EXIT_ONFAIL 1
#include "../testSuite.h"

namespace tests::http::response
{
  TEST_CASE(codes, {
    ::http::response res{200, "OKAY"};
    ASSERT_EQ_INT(res.get_status_code(), 200, "status code");
    ASSERT_EQ_CHAR(res.get_reason_phrase(), "OKAY", "phrase");
  });

  TEST_CASE(wrong_codes, {
    ::http::response res{200, "OK"};

    res.with_status(-1);
    ASSERT_EQ_INT(res.get_status_code(), 500, "negative code");

    res.with_status(99);
    ASSERT_EQ_INT(res.get_status_code(), 500, "out of range (min)");

    res.with_status(600);
    ASSERT_EQ_INT(res.get_status_code(), 500, "out of range (max)");
  })

  TEST_CASE(protocol, {
    ::http::response res{200, "OK"};
    res.with_proto_ver("10.10");
    ASSERT_EQ_CHAR(res.get_proto_ver(), "HTTP/10.10", "protocol version");
  })

  TEST_CASE(with_header, {
    ::http::response res{200, "OK"};
    res.with_header("HeaderKeyReplaced", "HeaderValue");
    res.with_header("HeaderKey", "HeaderValue");
    ASSERT_EQ_CHAR(res.get_header("HeaderKey")->value, "HeaderValue", "replaced header");
    ASSERT_EQ_INT(res.get_headers().size(), 1, "headers size");
  });

  TEST_CASE(added_header, {
    ::http::response res{200, "OK"};
    res.with_added_header("HeaderKey", "HeaderValue");
    res.with_added_header("HeaderKey2", "HeaderValue2");
    ASSERT_EQ_CHAR(res.get_header("HeaderKey")->value, "HeaderValue", "appended header");
    ASSERT_EQ_INT(res.get_headers().size(), 2, "headers size");
  });

  TEST_CASE(get_header, {
    ::http::response res{200, "OK"};
    res.with_header("HeaderKey", "HeaderValue");
    ASSERT_EQ_CHAR(res.get_header("HeaderKey")->value, "HeaderKey", "found header");
    ASSERT_TRUE(res.get_header("Not found") == nullptr, "not found header");
  });

  TEST_CASE(has_header, {
    ::http::response res{200, "OK"};
    res.with_header("HeaderKey", "HeaderValue");
    res.with_header("HeaderKey2", "HeaderValue2");
    ASSERT_TRUE(res.has_header("HeaderKey2"), "has header");
  });

  TEST_CASE(unique_header, {
    ::http::response res{200, "OK"};
    res.with_added_header("HeaderKey", "HeaderValue");
    res.with_added_header("HeaderKey", "HeaderValue2");
    ASSERT_EQ_INT(res.get_headers().size(), 1, "headers size");
  });

  TEST_CASE(with_body, {
    ::http::response res{200, "OK"};
    res.with_body({'b', 'o', 'd', 'y'});
    ASSERT_EQ_CHAR(res.get_body().data(), "body", "body bytes");

    res.with_body("body_string");
    ASSERT_EQ_CHAR(res.get_body().data(), "body_string", "body string");
  });

  auto run() -> void
  {
    unique_header();
    wrong_codes();
    codes();
    protocol();
    with_header();
    added_header();
    has_header();
    with_body();
  }
} // namespace tests::http::response
