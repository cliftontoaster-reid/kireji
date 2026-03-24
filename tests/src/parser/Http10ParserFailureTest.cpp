#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

#include "parser/Http10Parser.hpp"
#include "parser/HttpTestHelpers.hpp"

TEST(Http10ParserFailureTest, RejectsUnsupportedHttpVersion) {
  Http10Parser parser;
  const std::string requestText = buildHttp10Request("GET / HTTP/1.1");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}

TEST(Http10ParserFailureTest, RejectsInvalidMethod) {
  Http10Parser parser;
  const std::string requestText = buildHttp10Request("BOGUS / HTTP/1.0");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}

TEST(Http10ParserFailureTest, RejectsMalformedRequestLineMissingPath) {
  Http10Parser parser;
  const std::string requestText = buildHttp10Request("GET HTTP/1.0");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}

TEST(Http10ParserFailureTest, RejectsMalformedRequestLineWithExtraTokens) {
  Http10Parser parser;
  const std::string requestText = buildHttp10Request("GET / HTTP/1.0 extra");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}

TEST(Http10ParserFailureTest, RejectsMalformedHeaderLineWithoutColon) {
  Http10Parser parser;
  const std::string requestText =
      buildHttp10Request("GET / HTTP/1.0", "Host example.com");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}

TEST(Http10ParserFailureTest, RejectsEmptyHeaderName) {
  Http10Parser parser;
  const std::string requestText =
      buildHttp10Request("GET / HTTP/1.0", ": value");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}

TEST(Http10ParserFailureTest, RejectsInvalidContentLengthValue) {
  Http10Parser parser;
  const std::string requestText = buildHttp10Request(
      "POST /upload HTTP/1.0", "Content-Length: nope", "body");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}

TEST(Http10ParserFailureTest, RejectsEmptyContentLengthValue) {
  Http10Parser parser;
  const std::string requestText =
      buildHttp10Request("POST /upload HTTP/1.0", "Content-Length:   ", "body");

  EXPECT_THROW(parser.feed(requestText.c_str(), requestText.size()),
               std::invalid_argument);
}
