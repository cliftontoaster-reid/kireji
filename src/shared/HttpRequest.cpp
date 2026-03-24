#include "shared/HttpRequest.hpp"

std::ostream& operator<<(std::ostream& os, HttpVersion version) {
  switch (version) {
    case HTTP_1_0:
      os << "HTTP/1.0";
      break;
    case HTTP_1_1:
      os << "HTTP/1.1";
      break;
    case HTTP_2_0:
      os << "HTTP/2.0";
      break;
    case HTTP_3_0:
      os << "HTTP/3.0";
      break;
  }
  return os;
}

std::string to_string(HttpVersion version) {
  switch (version) {
    case HTTP_1_0:
      return "HTTP/1.0";
    case HTTP_1_1:
      return "HTTP/1.1";
    case HTTP_2_0:
      return "HTTP/2.0";
    case HTTP_3_0:
      return "HTTP/3.0";
  }
  return "Unknown HTTP version";
}

HttpMethod parseHttpMethod(const std::string& method, HttpVersion version) {
  switch (method.size()) {
    case 3:
      if (method == "GET") return GET;
      if (method == "PUT" && version >= HTTP_1_1) return PUT;
      break;
    case 4:
      if (method == "POST") return POST;
      if (method == "HEAD") return HEAD;
      break;
    case 5:
      if (method == "PATCH" && version >= HTTP_1_1) return PATCH;
      break;
    case 6:
      if (method == "DELETE" && version >= HTTP_1_1) return DELETE;
      break;
    case 7:
      if (method == "OPTIONS" && version >= HTTP_1_1) return OPTIONS;
      break;
  }
  throw std::invalid_argument("Invalid " + to_string(version) +
                              " method: " + method);
}

HttpRequest::HttpRequest(HttpMethod m, const std::string& p, HttpVersion v)
    : method(m), path(p), version(v) {}

HttpRequest::HttpRequest(HttpMethod m, const std::string& p, HttpVersion v,
                         const std::string& b, const HeaderMap& h)
    : method(m),
      path(p),
      version(v),
      body(Option<std::string>::Some(b)),
      headers(h) {}

HttpRequest::HttpRequest(HttpMethod m, const std::string& p, HttpVersion v,
                         const HeaderMap& h)
    : method(m),
      path(p),
      version(v),
      body(Option<std::string>::None()),
      headers(h) {}

HttpRequest::~HttpRequest() {}