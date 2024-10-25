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
    ASSERT_TRUE(match_get, "get");

    const auto match_post = router.match("/post");
    ASSERT_TRUE(match_post, "post");

    const auto is_exist = router.match("not_found");
    ASSERT_FALSE(is_exist, "empty");
  });

  TEST_CASE(match_params, {
    ::http::router router;
    router.add("/", method::Get,
        [](request *req, response *res) {

        });

    router.add("/blog/\\w+", method::Get,
        [](request *req, response *res) {

        });

    router.add("/blog/articles/\\d+", method::Get,
        [](request *req, response *res) {

        });
    router.add("/news/\\D+", method::Get,
        [](request *req, response *res) {

        });

    router.add("/news/\\D+/\\d+", method::Get,
        [](request *req, response *res) {

        });

    ASSERT_TRUE(router.match("/") != nullptr, "/ matched");
    ASSERT_TRUE(router.match("/word") == nullptr, "/word not matched");
    ASSERT_TRUE(router.match("/1") == nullptr, "/1 not_matched");
    ASSERT_TRUE(router.match("/1/2") == nullptr, "/1/2 not_matched");
    ASSERT_TRUE(router.match("/blog/articles/1") != nullptr, "/blog/articles/1 matched");
    ASSERT_TRUE(router.match("/blog/articles/not_found") == nullptr, "/blog/articles/not_found not_matched");
    ASSERT_TRUE(router.match("/blog") == nullptr, "/blog not_matched");
    ASSERT_TRUE(router.match("/blog/1") != nullptr, "/blog/1 matched");
    ASSERT_TRUE(router.match("/news/1") == nullptr, "/news/1 not matched");
    ASSERT_TRUE(router.match("/news/slug") != nullptr, "/news/slug matched");
    ASSERT_TRUE(router.match("/news/long-slug/1") != nullptr, "/news/long-slug/1 matched");
    ASSERT_TRUE(router.match("/news/long-slug/1/fragment") == nullptr, "/news/long-slug/1/fragment not matched");
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
    match_params();
  }
} // namespace tests::http::router
