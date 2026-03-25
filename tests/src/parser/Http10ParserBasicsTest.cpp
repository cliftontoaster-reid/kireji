#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "parser/Http10Parser.hpp"
#include "parser/HttpTestHelpers.hpp"

#ifdef KIREJI_ENABLE_HTTP1_0

TEST(Http10ParserBasicsTest, ParsesMinimalRequestAndEncodesRoundTrip) {
  Http10Parser parser;
  const std::string requestText = buildHttp10Request("GET / HTTP/1.0");

  parser.feed(requestText.c_str(), requestText.size());

  ASSERT_TRUE(parser.hasRequest());
  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);

  EXPECT_EQ(GET, request->method);
  EXPECT_EQ("/", request->path);
  EXPECT_EQ(HTTP_1_0, request->version);
  EXPECT_TRUE(request->body.is_none());
  EXPECT_EQ(0u, request->headers.size());
  EXPECT_EQ(requestText, request->encode());
}

TEST(Http10ParserBasicsTest, ParsesMultipleHeadersCaseInsensitiveLookup) {
  Http10Parser parser;
  const std::string requestText = buildHttp10Request(
      "GET /headers HTTP/1.0",
      "Host: example.com\r\nUser-Agent: kireji\r\nAccept: */*", "");

  parser.feed(requestText.c_str(), requestText.size());

  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);

  EXPECT_EQ(3u, request->headers.size());

  const std::string* host = request->headers.get("host");
  ASSERT_TRUE(host != NULL);
  EXPECT_EQ("example.com", *host);

  const std::string* userAgent = request->headers.get("USER-AGENT");
  ASSERT_TRUE(userAgent != NULL);
  EXPECT_EQ("kireji", *userAgent);

  const std::string* accept = request->headers.get("Accept");
  ASSERT_TRUE(accept != NULL);
  EXPECT_EQ("*/*", *accept);
}

TEST(Http10ParserBasicsTest, ParsesZeroLengthBody) {
  Http10Parser parser;
  const std::string requestText =
      buildHttp10Request("POST /empty HTTP/1.0", "Content-Length: 0", "");

  parser.feed(requestText.c_str(), requestText.size());

  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);

  EXPECT_EQ(POST, request->method);
  EXPECT_TRUE(request->body.is_some());
  EXPECT_EQ(std::string(""), request->body.unwrap());
  EXPECT_EQ(1u, request->headers.size());
}

TEST(Http10ParserBasicsTest, OverwritesDuplicateHeaderValues) {
  Http10Parser parser;
  const std::string requestText =
      buildHttp10Request("GET /dup HTTP/1.0", "X-Test: one\r\nX-Test: two", "");

  parser.feed(requestText.c_str(), requestText.size());

  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);

  EXPECT_EQ(1u, request->headers.size());

  const std::string* value = request->headers.get("x-test");
  ASSERT_TRUE(value != NULL);
  EXPECT_EQ("two", *value);
}

TEST(Http10ParserBasicsTest, CanReuseParserAfterPoppingRequest) {
  Http10Parser parser;

  const std::string firstRequest = buildHttp10Request("GET /one HTTP/1.0");
  parser.feed(firstRequest.c_str(), firstRequest.size());

  std::unique_ptr<HttpRequest> first(parser.nextRequest());
  ASSERT_TRUE(first.get() != NULL);
  EXPECT_EQ("/one", first->path);

  const std::string secondRequest = buildHttp10Request("GET /two HTTP/1.0");
  parser.feed(secondRequest.c_str(), secondRequest.size());

  std::unique_ptr<HttpRequest> second(parser.nextRequest());
  ASSERT_TRUE(second.get() != NULL);
  EXPECT_EQ("/two", second->path);
}

#endif