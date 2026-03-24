#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "parser/Http10Parser.hpp"
#include "parser/HttpTestHelpers.hpp"

TEST(Http10ParserBufferingTest, WaitsForRequestLineCompletion) {
  Http10Parser parser;

  const std::string firstChunk = "GET / HT";
  const std::string secondChunk = "TP/1.0\r\n\r\n";

  parser.feed(firstChunk.c_str(), firstChunk.size());
  EXPECT_FALSE(parser.hasRequest());
  EXPECT_EQ(NULL, parser.nextRequest());

  parser.feed(secondChunk.c_str(), secondChunk.size());
  ASSERT_TRUE(parser.hasRequest());

  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);
  EXPECT_EQ("/", request->path);
}

TEST(Http10ParserBufferingTest, WaitsForHeadersCompletion) {
  Http10Parser parser;

  const std::string firstChunk =
      "GET /headers HTTP/1.0\r\nHost: example.com\r\n";
  const std::string secondChunk = "\r\n";

  parser.feed(firstChunk.c_str(), firstChunk.size());
  EXPECT_FALSE(parser.hasRequest());

  parser.feed(secondChunk.c_str(), secondChunk.size());
  ASSERT_TRUE(parser.hasRequest());

  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);
  EXPECT_EQ(1u, request->headers.size());
}

TEST(Http10ParserBufferingTest, WaitsForBodyCompletion) {
  Http10Parser parser;

  const std::string firstChunk =
      "POST /body HTTP/1.0\r\nContent-Length: 5\r\n\r\nhe";
  const std::string secondChunk = "llo";

  parser.feed(firstChunk.c_str(), firstChunk.size());
  EXPECT_FALSE(parser.hasRequest());

  parser.feed(secondChunk.c_str(), secondChunk.size());
  ASSERT_TRUE(parser.hasRequest());

  std::unique_ptr<HttpRequest> request(parser.nextRequest());
  ASSERT_TRUE(request.get() != NULL);
  ASSERT_TRUE(request->body.is_some());
  EXPECT_EQ("hello", request->body.unwrap());
}

TEST(Http10ParserBufferingTest, QueuesMultipleRequestsFromSingleFeed) {
  Http10Parser parser;

  const std::string requestText = buildHttp10Request("GET /one HTTP/1.0") +
                                  buildHttp10Request("GET /two HTTP/1.0");

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
}

TEST(Http10ParserBufferingTest, QueuesMultipleRequestsAcrossFeeds) {
  Http10Parser parser;

  const std::string firstRequest = buildHttp10Request("GET /one HTTP/1.0");
  const std::string secondRequest = buildHttp10Request("GET /two HTTP/1.0");

  parser.feed(firstRequest.c_str(), firstRequest.size());
  ASSERT_TRUE(parser.hasRequest());

  std::unique_ptr<HttpRequest> first(parser.nextRequest());
  ASSERT_TRUE(first.get() != NULL);
  EXPECT_EQ("/one", first->path);

  parser.feed(secondRequest.c_str(), secondRequest.size());
  ASSERT_TRUE(parser.hasRequest());

  std::unique_ptr<HttpRequest> second(parser.nextRequest());
  ASSERT_TRUE(second.get() != NULL);
  EXPECT_EQ("/two", second->path);
}

TEST(Http10ParserBufferingTest,
     KeepsIncompleteTailBufferedAfterCompleteRequest) {
  Http10Parser parser;

  const std::string firstChunk =
      buildHttp10Request("GET /one HTTP/1.0") + "GET /t";
  const std::string secondChunk = "wo HTTP/1.0\r\n\r\n";

  parser.feed(firstChunk.c_str(), firstChunk.size());

  ASSERT_TRUE(parser.hasRequest());
  std::unique_ptr<HttpRequest> first(parser.nextRequest());
  ASSERT_TRUE(first.get() != NULL);
  EXPECT_EQ("/one", first->path);

  EXPECT_FALSE(parser.hasRequest());

  parser.feed(secondChunk.c_str(), secondChunk.size());
  ASSERT_TRUE(parser.hasRequest());

  std::unique_ptr<HttpRequest> second(parser.nextRequest());
  ASSERT_TRUE(second.get() != NULL);
  EXPECT_EQ("/two", second->path);
}