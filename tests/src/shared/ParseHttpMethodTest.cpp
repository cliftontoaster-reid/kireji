#include <gtest/gtest.h>

#include <stdexcept>

#include "shared/HttpRequest.hpp"

TEST(ParseHttpMethodTest, AcceptsCommonHTTP10Methods) {
  EXPECT_EQ(GET, parseHttpMethod("GET", HTTP_1_0));
  EXPECT_EQ(POST, parseHttpMethod("POST", HTTP_1_0));
  EXPECT_EQ(HEAD, parseHttpMethod("HEAD", HTTP_1_0));
}

TEST(ParseHttpMethodTest, AcceptsHTTP11OnlyMethodsOnHTTP11) {
  EXPECT_EQ(PUT, parseHttpMethod("PUT", HTTP_1_1));
  EXPECT_EQ(DELETE, parseHttpMethod("DELETE", HTTP_1_1));
  EXPECT_EQ(OPTIONS, parseHttpMethod("OPTIONS", HTTP_1_1));
  EXPECT_EQ(PATCH, parseHttpMethod("PATCH", HTTP_1_1));
}

TEST(ParseHttpMethodTest, RejectsHTTP11OnlyMethodsOnHTTP10) {
  EXPECT_THROW(parseHttpMethod("PUT", HTTP_1_0), std::invalid_argument);
  EXPECT_THROW(parseHttpMethod("DELETE", HTTP_1_0), std::invalid_argument);
  EXPECT_THROW(parseHttpMethod("OPTIONS", HTTP_1_0), std::invalid_argument);
  EXPECT_THROW(parseHttpMethod("PATCH", HTTP_1_0), std::invalid_argument);
}

TEST(ParseHttpMethodTest, RejectsUnknownMethod) {
  EXPECT_THROW(parseHttpMethod("BOGUS", HTTP_1_0), std::invalid_argument);
}