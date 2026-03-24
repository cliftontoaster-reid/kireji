#pragma once

#include <cstddef>

#include "HttpRequest.hpp"

class IHttpParser {
 public:
  virtual ~IHttpParser();

  // Feed raw bytes from the TCP stream.
  // Packet boundaries are not part of the parser contract.
  virtual void feed(const char* data, size_t size) = 0;

  // Returns true when a full request is ready to be popped.
  virtual bool hasRequest() const = 0;

  // Returns the oldest completed request.
  // The caller owns the returned object.
  virtual HttpRequest* nextRequest() = 0;
};
