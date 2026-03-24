#include <cstring>

#include "shared/HttpRequest.hpp"

static size_t findNthSpace(const char* data, size_t size, int n) {
  size_t pos = 0;
  while (pos < size && n > 0) {
    if (data[pos] == ' ') n--;
    pos++;
  }
  return pos;
}

HttpVersion findHttpVersion(const char* data, size_t size) {
  // We need to find the version in "{METHOD} {PATH} HTTP/{VERSION}\r\n"
  // And so we need to skip the first two tokens (method and path)
  size_t pos = findNthSpace(data, size, 2);

  if (pos + 8 > size) {
    throw std::runtime_error("Invalid HTTP request: version not found");
  }

  struct VersionMapping {
    const char* str;
    HttpVersion version;
    const char* macro;
  };

  const VersionMapping versions[] = {
      {"HTTP/1.0", HTTP_1_0, "KIREJI_ENABLE_HTTP1_0"},
      {"HTTP/1.1", HTTP_1_1, "KIREJI_ENABLE_HTTP1_1"},
      {"HTTP/2.0", HTTP_2_0, "KIREJI_ENABLE_HTTP2_0"},
      {"HTTP/3.0", HTTP_3_0, "KIREJI_ENABLE_HTTP3_0"},
  };

  for (size_t i = 0; i < sizeof(versions) / sizeof(versions[0]); ++i) {
    const VersionMapping& mapping = versions[i];
    if (std::strncmp(data + pos, mapping.str, 8) == 0) {
#ifdef KIREJI_ENABLE_HTTP1_0
      if (std::strcmp(mapping.macro, "KIREJI_ENABLE_HTTP1_0") == 0)
        return mapping.version;
#endif
#ifdef KIREJI_ENABLE_HTTP1_1
      if (std::strcmp(mapping.macro, "KIREJI_ENABLE_HTTP1_1") == 0)
        return mapping.version;
#endif
#ifdef KIREJI_ENABLE_HTTP2_0
      if (std::strcmp(mapping.macro, "KIREJI_ENABLE_HTTP2_0") == 0)
        return mapping.version;
#endif
#ifdef KIREJI_ENABLE_HTTP3_0
      if (std::strcmp(mapping.macro, "KIREJI_ENABLE_HTTP3_0") == 0)
        return mapping.version;
#endif
      throw std::runtime_error(std::string(mapping.str) + " is not supported");
    }
  }

  throw std::runtime_error("Invalid HTTP version");
}

HttpRequest* parseHttpRequest(const char* data, size_t size) {
  HttpVersion version = findHttpVersion(data, size);
  HttpMethod method =
      parseHttpMethod(std::string(data, findNthSpace(data, size, 1)), version);

  switch (version) {
    default:
      throw std::runtime_error("Unsupported HTTP version");
  }
}