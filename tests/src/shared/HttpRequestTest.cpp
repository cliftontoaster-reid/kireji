#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "shared/HttpRequest.hpp"

class DummyHttpRequest : public HttpRequest {
 public:
  DummyHttpRequest(HttpMethod method, const std::string& path,
                   HttpVersion version)
      : HttpRequest(method, path, version) {}

  DummyHttpRequest(HttpMethod method, const std::string& path,
                   HttpVersion version, const std::string& body,
                   const HeaderMap& headers)
      : HttpRequest(method, path, version, body, headers) {}

  DummyHttpRequest(HttpMethod method, const std::string& path,
                   HttpVersion version, const HeaderMap& headers)
      : HttpRequest(method, path, version, headers) {}

  HttpRequest* decode(const char* data, size_t size) {
    (void)data;
    (void)size;
    return NULL;
  }

  std::string encode() const { return path; }
};

TEST(HttpRequestTest, ConstructorWithoutBodyInitializesFields) {
  DummyHttpRequest request(GET, "/", HTTP_1_0);

  EXPECT_EQ(GET, request.method);
  EXPECT_EQ("/", request.path);
  EXPECT_EQ(HTTP_1_0, request.version);
  EXPECT_TRUE(request.body.is_none());
  EXPECT_EQ(0u, request.headers.size());
}

TEST(HttpRequestTest, ConstructorWithBodyStoresBodyAndHeaders) {
  HeaderMap headers;
  headers.set("Host", "example.com");

  DummyHttpRequest request(POST, "/submit", HTTP_1_0, "payload", headers);

  EXPECT_EQ(POST, request.method);
  EXPECT_EQ("/submit", request.path);
  ASSERT_TRUE(request.body.is_some());
  EXPECT_EQ("payload", request.body.unwrap());
  ASSERT_TRUE(request.headers.get("host") != NULL);
  EXPECT_EQ("example.com", *request.headers.get("HOST"));
}

TEST(HttpRequestTest, ConstructorWithHeadersCopiesHeaderMap) {
  HeaderMap headers;
  headers.set("A", "1");

  DummyHttpRequest request(GET, "/copy", HTTP_1_0, headers);
  headers.set("A", "2");

  ASSERT_TRUE(request.headers.get("a") != NULL);
  EXPECT_EQ("1", *request.headers.get("a"));
}

TEST(HttpRequestTest, ToStringFormatsVersions) {
  EXPECT_EQ("HTTP/1.0", to_string(HTTP_1_0));
  EXPECT_EQ("HTTP/1.1", to_string(HTTP_1_1));
  EXPECT_EQ("HTTP/2.0", to_string(HTTP_2_0));
  EXPECT_EQ("HTTP/3.0", to_string(HTTP_3_0));
}

TEST(HttpRequestTest, StreamOperatorFormatsVersions) {
  std::ostringstream stream;

  stream << HTTP_1_1;

  EXPECT_EQ("HTTP/1.1", stream.str());
}

TEST(HttpRequestTest, DummyEncodeCanBeCalledThroughVirtualInterface) {
  DummyHttpRequest request(GET, "/encode", HTTP_1_0);
  HttpRequest* base = &request;

  EXPECT_EQ("/encode", base->encode());
}

TEST(HttpRequestTest, ParseHttpMethodRejectsUnsupportedMethodForVersion) {
  EXPECT_THROW(parseHttpMethod("PUT", HTTP_1_0), std::invalid_argument);
}

TEST(HttpRequestTest, ParseHttpMethodAcceptsVersionSpecificMethods) {
  EXPECT_EQ(PUT, parseHttpMethod("PUT", HTTP_1_1));
  EXPECT_EQ(DELETE, parseHttpMethod("DELETE", HTTP_1_1));
}