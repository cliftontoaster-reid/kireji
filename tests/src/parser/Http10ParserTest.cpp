#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "parser/Http10Parser.hpp"

static std::string makeRequest(const std::string& requestLine,
                               const std::string& headers,
                               const std::string& body) {
  std::string request = requestLine;
  request += "\r\n";
  if (headers.empty()) {
    request += "\r\n";
  } else {
    request += headers;
    request += "\r\n\r\n";
  }
  request += body;
  return request;
}

TEST(Http10ParserTest, NextRequestReturnsNullWhenEmpty) {
  Http10Parser parser;

  EXPECT_FALSE(parser.hasRequest());
  EXPECT_EQ(NULL, parser.nextRequest());
}

TEST(Http10ParserTest, ParsesSingleRequestWithoutHeaders) {
  Http10Parser parser;
  const std::string requestText = makeRequest("GET / HTTP/1.0", "", "");

  parser.feed(requestText.c_str(), requestText.size());

  EXPECT_TRUE(parser.hasRequest());
  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);

  EXPECT_EQ(GET, request->method);
  EXPECT_EQ("/", request->path);
  EXPECT_EQ(HTTP_1_0, request->version);
  EXPECT_TRUE(request->body.is_none());
  EXPECT_EQ(0u, request->headers.size());
  EXPECT_FALSE(parser.hasRequest());
}

TEST(Http10ParserTest, ParsesHeadersAndBody) {
  Http10Parser parser;
  const std::string requestText = makeRequest(
      "POST /submit HTTP/1.0", "Host: example.com\r\nContent-Length: 3", "abc");

  parser.feed(requestText.c_str(), requestText.size());

  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);

  EXPECT_EQ(POST, request->method);
  EXPECT_EQ("/submit", request->path);
  EXPECT_EQ(HTTP_1_0, request->version);
  ASSERT_TRUE(request->body.is_some());
  EXPECT_EQ(std::string("abc"), request->body.unwrap());
  EXPECT_EQ(2u, request->headers.size());

  const std::string* host = request->headers.get("Host");
  ASSERT_TRUE(host != NULL);
  EXPECT_EQ("example.com", *host);

  const std::string* contentLength = request->headers.get("content-length");
  ASSERT_TRUE(contentLength != NULL);
  EXPECT_EQ("3", *contentLength);
}

TEST(Http10ParserTest, BuffersPartialRequestUntilComplete) {
  Http10Parser parser;
  const std::string firstChunk = "GET / HTTP/1.0\r\nHost: example.com\r\n";
  const std::string secondChunk = "\r\n";

  parser.feed(firstChunk.c_str(), firstChunk.size());
  EXPECT_FALSE(parser.hasRequest());
  EXPECT_EQ(NULL, parser.nextRequest());

  parser.feed(secondChunk.c_str(), secondChunk.size());
  ASSERT_TRUE(parser.hasRequest());

  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);

  EXPECT_EQ(GET, request->method);
  EXPECT_EQ("/", request->path);
  EXPECT_EQ(HTTP_1_0, request->version);
  EXPECT_TRUE(request->body.is_none());
  EXPECT_EQ(1u, request->headers.size());
}

TEST(Http10ParserTest, BuffersBodyUntilComplete) {
  Http10Parser parser;
  const std::string firstChunk =
      "POST /upload HTTP/1.0\r\nContent-Length: 5\r\n\r\nhe";
  const std::string secondChunk = "llo";

  parser.feed(firstChunk.c_str(), firstChunk.size());
  EXPECT_FALSE(parser.hasRequest());
  EXPECT_EQ(NULL, parser.nextRequest());

  parser.feed(secondChunk.c_str(), secondChunk.size());
  ASSERT_TRUE(parser.hasRequest());

  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);

  ASSERT_TRUE(request->body.is_some());
  EXPECT_EQ(std::string("hello"), request->body.unwrap());
  EXPECT_FALSE(parser.hasRequest());
}

TEST(Http10ParserTest, QueuesMultipleRequestsFromOneFeed) {
  Http10Parser parser;
  const std::string requestText = makeRequest("GET /one HTTP/1.0", "", "") +
                                  makeRequest("GET /two HTTP/1.0", "", "");

  parser.feed(requestText.c_str(), requestText.size());

  ASSERT_TRUE(parser.hasRequest());
  std::unique_ptr<HttpRequest> first(parser.nextRequest());
  ASSERT_TRUE(first.get() != NULL);
  EXPECT_EQ("/one", first->path);

  ASSERT_TRUE(parser.hasRequest());
  std::unique_ptr<HttpRequest> second(parser.nextRequest());
  ASSERT_TRUE(second.get() != NULL);
  EXPECT_EQ("/two", second->path);

  EXPECT_FALSE(parser.hasRequest());
  EXPECT_EQ(NULL, parser.nextRequest());
}

TEST(Http10ParserTest, RejectsInvalidMethod) {
  Http10Parser parser;
  const std::string requestText = makeRequest("BOGUS / HTTP/1.0", "", "");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}

TEST(Http10ParserTest, RejectsInvalidContentLength) {
  Http10Parser parser;
  const std::string requestText =
      makeRequest("POST /upload HTTP/1.0", "Content-Length: nope", "body");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}
