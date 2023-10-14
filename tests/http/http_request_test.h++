#pragma once

#include <http/request.h++>

#undef TEST_EXIT_ONFAIL
#define TEST_EXIT_ONFAIL 0
#include "../testSuite.h"

namespace tests::http::request
{
  TEST_CASE(parse, {
    const auto request = new ::http::request();
    auto result = request->parse("GET / HTTP/2");
    ASSERT_EQ_CHAR("/", result->uri.c_str(), "uri");
    ASSERT_EQ_FLOAT(2, result->http_ver, "version");
    ASSERT_TRUE(result->method == ::http::request::methods::Get, "method");
    delete request;

    const auto bad_parse = new ::http::request();
    auto bad_result = bad_parse->parse("3434 /");
    ASSERT_TRUE(bad_result == nullptr, "bad request");
    delete bad_parse;
  });

  TEST_CASE(validate_request, {
    ::http::request::http_request req;
    req.http_ver = 1.1;
    req.uri = "/";
    req.method = ::http::request::methods::Get;
    ASSERT_TRUE(::http::request::validate_request(&req), "is valid");

    ::http::request::http_request bad_req;
    bad_req.http_ver = 0;
    bad_req.uri = "";
    bad_req.method = ::http::request::methods::Get;
    ASSERT_FALSE(::http::request::validate_request(&bad_req), "not valid");
  })

  TEST_CASE(parse_methods, {
    auto get = ::http::request::get_method("GET");
    ASSERT_TRUE(get == ::http::request::methods::Get, "GET");

    auto post = ::http::request::get_method("post");
    ASSERT_TRUE(post == ::http::request::methods::Post, "POST");

    auto patch = ::http::request::get_method("PaTch");
    ASSERT_TRUE(patch == ::http::request::methods::Patch, "PATCH");

    auto put = ::http::request::get_method("puT ");
    ASSERT_TRUE(put == ::http::request::methods::Put, "PUT");

    auto d = ::http::request::get_method(" DelETe ");
    ASSERT_TRUE(d == ::http::request::methods::Delete, "Delete");

    auto connect = ::http::request::get_method(" connect");
    ASSERT_TRUE(connect == ::http::request::methods::Connect, "connect");

    auto trace = ::http::request::get_method("trace");
    ASSERT_TRUE(trace == ::http::request::methods::Trace, "trace");
  })

  TEST_CASE(parse_version, {
    auto result = ::http::request::get_version("HTTP/1.1");
    ASSERT_EQ_FLOAT(result, 1.1, "http");

    auto r2 = ::http::request::get_version("HTTP/2");
    ASSERT_EQ_FLOAT(r2, 2, "http 2");

    auto r3 = ::http::request::get_version("HTTP/3");
    ASSERT_EQ_FLOAT(r3, 3, "http 3");

    auto bad = ::http::request::get_version("HTTP/BAD_VER");
    ASSERT_EQ_FLOAT(bad, 0, "http bad");

    auto bad_proto = ::http::request::get_version("BAD_PROTO");
    ASSERT_EQ_FLOAT(bad_proto, 0, "http bad proto");
  })

  TEST_CASE(parse_uri, {
    auto r = ::http::request::get_uri("/uri");
    ASSERT_EQ_CHAR("/uri", r.c_str(), "uri");

    auto r2 = ::http::request::get_uri("");
    ASSERT_EQ_CHAR("", r2.c_str(), "uri empty");

    auto r3 = ::http::request::get_uri("/uri/path/2?parm=1&key=value");
    ASSERT_EQ_CHAR("/uri/path/2?parm=1&key=value", r3.c_str(), "uri long");

    auto r4 = ::http::request::get_uri(" /uri/path ");
    ASSERT_EQ_CHAR("/uri/path", r4.c_str(), "uri trimmed");
  })

  auto run() -> void
  {
    parse_methods();
    parse_version();
    parse_uri();
    parse();
  }
} // namespace tests::http::request
