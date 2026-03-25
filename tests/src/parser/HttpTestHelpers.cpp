#include "parser/HttpTestHelpers.hpp"

#ifdef KIREJI_ENABLE_HTTP1_0

std::string buildHttp10Request(const std::string& requestLine,
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

#endif