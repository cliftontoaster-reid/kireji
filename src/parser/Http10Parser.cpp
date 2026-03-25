#include "parser/Http10Parser.hpp"

#include <stdexcept>

#ifdef KIREJI_ENABLE_HTTP1_0

static std::string trimLeft(const std::string& value) {
  std::size_t index = 0;
  while (index < value.size() &&
         (value[index] == ' ' || value[index] == '\t')) {
    ++index;
  }
  return value.substr(index);
}

static std::string trimRight(const std::string& value) {
  std::size_t end = value.size();
  while (end > 0 && (value[end - 1] == ' ' || value[end - 1] == '\t')) {
    --end;
  }
  return value.substr(0, end);
}

static std::string trim(const std::string& value) {
  return trimRight(trimLeft(value));
}

static std::string methodToString(HttpMethod method) {
  switch (method) {
    case GET:
      return "GET";
    case POST:
      return "POST";
    case PUT:
      return "PUT";
    case DELETE:
      return "DELETE";
    case HEAD:
      return "HEAD";
    case OPTIONS:
      return "OPTIONS";
    case PATCH:
      return "PATCH";
  }
  throw std::runtime_error("Unknown HTTP method");
}

static bool parseContentLength(const std::string& value, std::size_t& length) {
  std::string cleaned = trim(value);
  if (cleaned.empty()) return false;

  std::size_t result = 0;
  for (std::size_t i = 0; i < cleaned.size(); ++i) {
    if (cleaned[i] < '0' || cleaned[i] > '9') return false;
    result = result * 10 + static_cast<std::size_t>(cleaned[i] - '0');
  }

  length = result;
  return true;
}

static std::string getHeaderValue(const HeaderMap& headers,
                                  const std::string& name) {
  const std::string* value = headers.get(name);
  if (!value) return std::string();
  return *value;
}

static HttpRequest* decodeHttp10Request(const char* data, std::size_t size,
                                        std::size_t& consumed);

class BasicHttp10Request : public HttpRequest {
 public:
  BasicHttp10Request(HttpMethod method, const std::string& path,
                     HttpVersion version, const HeaderMap& headers)
      : HttpRequest(method, path, version, headers) {}

  BasicHttp10Request(HttpMethod method, const std::string& path,
                     HttpVersion version, const std::string& body,
                     const HeaderMap& headers)
      : HttpRequest(method, path, version, body, headers) {}

  HttpRequest* decode(const char* data, std::size_t size) {
    std::size_t consumed = 0;
    HttpRequest* request = decodeHttp10Request(data, size, consumed);
    if (!request) {
      throw std::runtime_error("Incomplete HTTP/1.0 request");
    }
    if (consumed != size) {
      delete request;
      throw std::runtime_error("HTTP/1.0 decode expects exactly one request");
    }
    return request;
  }

  std::string encode() const {
    std::string encoded = methodToString(method);
    encoded += " ";
    encoded += path;
    encoded += " ";
    encoded += to_string(version);
    encoded += "\r\n";

    for (HeaderMap::Node* node = headers.head(); node; node = node->next) {
      encoded += node->name;
      encoded += ": ";
      encoded += node->value;
      encoded += "\r\n";
    }

    encoded += "\r\n";
    if (body.is_some()) {
      encoded += body.unwrap();
    }

    return encoded;
  }
};

static HttpRequest* decodeHttp10Request(const char* data, std::size_t size,
                                        std::size_t& consumed) {
  consumed = 0;

  if (!data || size == 0) {
    return NULL;
  }

  std::size_t lineEnd = 0;
  while (lineEnd + 1 < size) {
    if (data[lineEnd] == '\r' && data[lineEnd + 1] == '\n') break;
    ++lineEnd;
  }
  if (lineEnd + 1 >= size) return NULL;

  std::string requestLine(data, lineEnd);
  std::size_t firstSpace = requestLine.find(' ');
  if (firstSpace == std::string::npos) {
    throw std::invalid_argument("Invalid HTTP/1.0 request line");
  }

  std::size_t secondSpace = requestLine.find(' ', firstSpace + 1);
  if (secondSpace == std::string::npos ||
      requestLine.find(' ', secondSpace + 1) != std::string::npos) {
    throw std::invalid_argument("Invalid HTTP/1.0 request line");
  }

  std::string methodToken = requestLine.substr(0, firstSpace);
  std::string path =
      requestLine.substr(firstSpace + 1, secondSpace - firstSpace - 1);
  std::string versionToken = requestLine.substr(secondSpace + 1);

  if (versionToken != "HTTP/1.0") {
    throw std::invalid_argument("Only HTTP/1.0 is supported by Http10Parser");
  }

  HttpVersion version = HTTP_1_0;
  HttpMethod method = parseHttpMethod(methodToken, version);

  HeaderMap headers;
  std::size_t position = lineEnd + 2;

  while (true) {
    if (position + 1 >= size) return NULL;

    std::size_t headerEnd = position;
    while (headerEnd + 1 < size) {
      if (data[headerEnd] == '\r' && data[headerEnd + 1] == '\n') break;
      ++headerEnd;
    }

    if (headerEnd + 1 >= size) return NULL;

    if (headerEnd == position) {
      position += 2;
      break;
    }

    std::string headerLine(data + position, headerEnd - position);
    std::size_t colon = headerLine.find(':');
    if (colon == std::string::npos || colon == 0) {
      throw std::invalid_argument("Invalid HTTP header line");
    }

    std::string name = trim(headerLine.substr(0, colon));
    std::string value = trimLeft(headerLine.substr(colon + 1));
    headers.set(name, value);

    position = headerEnd + 2;
  }

  std::size_t bodySize = 0;
  bool hasBody = false;
  const std::string* contentLength = headers.get("Content-Length");
  if (contentLength != NULL) {
    if (!parseContentLength(*contentLength, bodySize)) {
      throw std::invalid_argument("Invalid Content-Length header");
    }
    hasBody = true;
  }

  if (size < position + bodySize) {
    return NULL;
  }

  consumed = position + bodySize;

  if (hasBody) {
    return new BasicHttp10Request(
        method, path, version, std::string(data + position, bodySize), headers);
  }

  return new BasicHttp10Request(method, path, version, headers);
}

static bool tryParseRequest(const std::string& buffer, HttpRequest*& request,
                            std::size_t& consumed) {
  request = NULL;
  consumed = 0;

  request = decodeHttp10Request(buffer.c_str(), buffer.size(), consumed);
  return request != NULL;
}

Http10Parser::Http10Parser() : buffer_(""), requests_() {}

Http10Parser::~Http10Parser() throw() { clearRequests(); }

void Http10Parser::clearRequests() {
  while (!requests_.empty()) {
    delete requests_.pop_front();
  }
}

void Http10Parser::drainBuffer() {
  while (true) {
    HttpRequest* request = NULL;
    std::size_t consumed = 0;

    if (!tryParseRequest(buffer_, request, consumed)) {
      return;
    }

    requests_.push_back(request);
    buffer_.erase(0, consumed);
  }
}

void Http10Parser::feed(const char* data, size_t size) {
  if (!data || size == 0) return;

  buffer_.append(data, size);
  drainBuffer();
}

bool Http10Parser::hasRequest() const { return !requests_.empty(); }

HttpRequest* Http10Parser::nextRequest() {
  if (requests_.empty()) {
    return NULL;
  }

  return requests_.pop_front();
}

#endif