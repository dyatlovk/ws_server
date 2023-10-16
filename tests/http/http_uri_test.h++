#pragma once

#include <http/uri.h++>

#undef TEST_EXIT_ONFAIL
#define TEST_EXIT_ONFAIL 0
#include "../testSuite.h"

namespace tests::http::uri
{
  TEST_CASE(empty, {
    auto uri = new ::http::uri();
    auto r = uri->parse("");
    ASSERT_EQ_CHAR("", r.scheme.c_str(), "scheme");
    ASSERT_EQ_CHAR("", r.host.c_str(), "host");
    ASSERT_EQ_INT(0, r.port, "port");
    ASSERT_EQ_CHAR("/", r.path.c_str(), "path");
    ASSERT_EQ_CHAR("", r.query.c_str(), "query");
    ASSERT_EQ_CHAR("", r.fragment.c_str(), "fragment");
    delete uri;
  })

  TEST_CASE(scheme_authority, {
    auto uri = new ::http::uri();
    auto u = uri->parse("http://host.com:443");
    ASSERT_EQ_CHAR("http", u.scheme.c_str(), "uri");
    ASSERT_EQ_CHAR("host.com", u.host.c_str(), "host");
    ASSERT_EQ_CHAR("/", u.path.c_str(), "path");
    ASSERT_EQ_INT(443, u.port, "port");
    delete uri;
  })

  TEST_CASE(scheme_https, {
    auto uri = new ::http::uri();
    auto u = uri->parse("https://host.com");
    ASSERT_EQ_CHAR("https", u.scheme.c_str(), "uri");
    ASSERT_EQ_CHAR("host.com", u.host.c_str(), "host");
    delete uri;
  })

  TEST_CASE(scheme_ws, {
    auto uri = new ::http::uri();
    auto u = uri->parse("ws://host.com");
    ASSERT_EQ_CHAR("ws", u.scheme.c_str(), "uri");
    ASSERT_EQ_CHAR("host.com", u.host.c_str(), "host");
    delete uri;
  })

  TEST_CASE(host, {
    auto uri = new ::http::uri();
    auto u = uri->parse("http://host.com");
    ASSERT_EQ_CHAR("http", u.scheme.c_str(), "uri");
    ASSERT_EQ_CHAR("host.com", u.host.c_str(), "host");
    ASSERT_EQ_INT(0, u.port, "port");
    delete uri;
  })

  TEST_CASE(uri_max, {
    auto uri = new ::http::uri();
    auto u = uri->parse("http://host.com:443/path?key=value&test=1#index");
    ASSERT_EQ_CHAR("http", u.scheme.c_str(), "uri");
    ASSERT_EQ_CHAR("host.com", u.host.c_str(), "host");
    ASSERT_EQ_CHAR("/path", u.path.c_str(), "path");
    ASSERT_EQ_INT(443, u.port, "port");
    ASSERT_EQ_CHAR("key=value&test=1", u.query.c_str(), "query");
    ASSERT_EQ_CHAR("index", u.fragment.c_str(), "fragment");
    delete uri;
  })

  TEST_CASE(uri_fragment, {
    auto uri = new ::http::uri();
    auto u = uri->parse("http://host.com/#index");
    ASSERT_EQ_CHAR("http", u.scheme.c_str(), "uri");
    ASSERT_EQ_CHAR("host.com", u.host.c_str(), "host");
    ASSERT_EQ_CHAR("/", u.path.c_str(), "path");
    ASSERT_EQ_INT(0, u.port, "port");
    ASSERT_EQ_CHAR("index", u.fragment.c_str(), "fragment");
    delete uri;
  })

  TEST_CASE(parse_scheme, {
    auto uri = new ::http::uri();
    auto s = uri->parse_scheme("https://localhost.com:80/path");
    ASSERT_EQ_INT(5, s, "scheme ok");

    auto bad = uri->parse_scheme("1http://host.com/path");
    ASSERT_EQ_INT(0, bad, "bad scheme");

    delete uri;
  })

  TEST_CASE(parse_regname, {
    auto uri = new ::http::uri();

    int pos_1 = uri->parse_regname("host.com/path");
    ASSERT_EQ_INT(8, pos_1, "host with path");

    int pos_2 = uri->parse_regname("host.com:80/path");
    ASSERT_EQ_INT(9, pos_2, "path with port 80");

    int pos_3 = uri->parse_regname("host.com:443");
    ASSERT_EQ_INT(9, pos_3, "host with port 443");

    int pos_4 = uri->parse_regname("host.com");
    ASSERT_EQ_INT(8, pos_4, "host only");

    delete uri;
  })

  TEST_CASE(parse_port, {
    auto uri = new ::http::uri();

    int pos_1 = uri->parse_port("443/path");
    ASSERT_EQ_INT(3, pos_1, "with path");

    int pos_2 = uri->parse_port("443");
    ASSERT_EQ_INT(3, pos_2, "without path");

    int pos_3 = uri->parse_port("443ddd");
    ASSERT_EQ_INT(6, pos_3, "invalid");

    delete uri;
  })

  TEST_CASE(parse_path, {
    auto uri = new ::http::uri();

    int pos_1 = uri->parse_path("/path");
    ASSERT_EQ_INT(5, pos_1, "with path");

    int pos_2 = uri->parse_path("/path/123");
    ASSERT_EQ_INT(9, pos_2, "long path");

    int pos_3 = uri->parse_path("/path/123/#fragment");
    ASSERT_EQ_INT(10, pos_3, "fragment end");

    int pos_4 = uri->parse_path("");
    ASSERT_EQ_INT(0, pos_4, "empty");

    int pos_5 = uri->parse_path("/path/123/?query=true");
    ASSERT_EQ_INT(10, pos_5, "query");

    delete uri;
  })

  TEST_CASE(parse_fragment, {
    auto uri = new ::http::uri();

    int pos_1 = uri->parse_fragment("#index");
    ASSERT_EQ_INT(6, pos_1, "fragment");

    int pos_2 = uri->parse_fragment("index");
    ASSERT_EQ_INT(0, pos_2, "bad");

    delete uri;
  })

  TEST_CASE(parse_query, {
    auto uri = new ::http::uri();

    int pos_1 = uri->parse_query("?key=value&test=1#index");
    ASSERT_EQ_INT(17, pos_1, "with fragment");

    int pos_2 = uri->parse_query("?key=value&test=1");
    ASSERT_EQ_INT(17, pos_2, "query");

    int pos_3 = uri->parse_query("key=value");
    ASSERT_EQ_INT(0, pos_3, "bad");

    delete uri;
  })

  TEST_CASE(parse_authority, {
    auto uri = new ::http::uri();

    int pos_1 = uri->parse_authority("host.com:443/path");
    ASSERT_EQ_INT(12, pos_1, "autority 1");

    int pos_2 = uri->parse_authority("host.com:443/path?query=true#index");
    ASSERT_EQ_INT(12, pos_2, "autority 2");

    int pos_3 = uri->parse_authority("host.com");
    ASSERT_EQ_INT(8, pos_3, "autority 3");

    delete uri;
  })

  TEST_CASE(parse_hier_part, {
    auto uri = new ::http::uri();

    int pos_1 = uri->parse_hier("//example.com:8042/over/there");
    ASSERT_EQ_INT(29, pos_1, "hier part");

    int pos_2 = uri->parse_hier("//example.com:8042/over/there?query=true");
    ASSERT_EQ_INT(29, pos_2, "hier part query");

    int pos_3 = uri->parse_hier("//example.com/path?query=true#fragment");
    ASSERT_EQ_INT(18, pos_3, "hier part fragment");

    int pos_4 = uri->parse_hier("example.com/path?query=true#fragment");
    ASSERT_EQ_INT(0, pos_4, "hier bad input");

    delete uri;
  })

  TEST_CASE(parse_host, {
    auto uri = new ::http::uri();

    int pos_1 = uri->parse_host("host.com:443/path");
    ASSERT_EQ_INT(9, pos_1, "host");
    delete uri;
  })

  auto run() -> void
  {
    parse_host();
    parse_regname();
    parse_port();
    parse_path();
    parse_fragment();
    parse_query();
    parse_authority();
    parse_hier_part();
    parse_scheme();
    //
    empty();
    scheme_authority();
    uri_max();
    host();
    scheme_https();
    scheme_ws();
    uri_fragment();
  }
} // namespace tests::http::uri
