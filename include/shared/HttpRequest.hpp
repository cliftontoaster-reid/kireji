#pragma once

#include <ostream>
#include <stdexcept>
#include <string>

#include "HeaderMap.hpp"
#include "Option.hpp"

enum HttpVersion {
  HTTP_1_0 = 0,
  HTTP_1_1 = 1,
  HTTP_2_0 = 2,
  HTTP_3_0 = 3,
};

std::ostream& operator<<(std::ostream& os, HttpVersion version);
std::string to_string(HttpVersion version);

enum HttpMethod {
  GET,
  POST,
  PUT,
  DELETE,
  HEAD,
  OPTIONS,
  PATCH,
};

HttpMethod parseHttpMethod(const std::string& method, HttpVersion version);

class HttpRequest {
 public:
  HttpMethod method;
  std::string path;
  HttpVersion version;
  Option<std::string> body;
  HeaderMap headers;

  HttpRequest(HttpMethod m, const std::string& p, HttpVersion v);
  HttpRequest(HttpMethod m, const std::string& p, HttpVersion v,
              const std::string& b, const HeaderMap& h);
  HttpRequest(HttpMethod m, const std::string& p, HttpVersion v,
              const HeaderMap& h);
  virtual ~HttpRequest();

  virtual HttpRequest* decode(const char* data, size_t size) = 0;
  virtual std::string encode() const = 0;
};

HttpRequest* parseHttpRequest(const char* data, size_t size);