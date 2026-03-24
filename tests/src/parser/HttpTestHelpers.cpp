#include "parser/HttpTestHelpers.hpp"

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