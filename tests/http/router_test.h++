#pragma once

#include <http/router.h++>
#undef TEST_EXIT_ONFAIL
#define TEST_EXIT_ONFAIL 1
#include "../testSuite.h"

namespace tests::http::router
{
  using request = ::http::request;
  using response = ::http::response;
  using method = request::methods;

  TEST_CASE(add, {
    ::http::router router;
    router.add("/", method::Get,
        [](request *req, response *res) {

        });
    router.add("/endpoint", {method::Post, method::Get},
        [](request *req, response *res) {

        });
    ASSERT_EQ_INT(router.get_size(), 2, "size");
  });

  TEST_CASE(match, {
    ::http::router router;
    router.add("/get", method::Get,
        [](request *req, response *res) {

        });
    router.add("/post", method::Post,
        [](request *req, response *res) {

        });
    const auto match_get = router.match("/get");
    ASSERT_TRUE(match_get, "get matched");

    const auto match_post = router.match("/post");
    ASSERT_TRUE(match_post, "post matched");

    const auto empty = router.match("not_found");
    ASSERT_FALSE(empty, "empty");
  });

  TEST_CASE(unique_endpoint, {
    ::http::router router;
    router.add("/", method::Get,
        [](request *req, response *res) {

        });
    router.add("/", method::Get,
        [](request *req, response *res) {

        });
    ASSERT_EQ_INT(router.get_size(), 1, "url and methods are same");

    router.add("/", {method::Get, method::Patch},
        [](request *req, response *res) {

        });
    ASSERT_EQ_INT(router.get_size(), 2, "with multiple methods");
  });

  auto run() -> void
  {
    add();
    match();
    unique_endpoint();
  }
} // namespace tests::http::router
