#pragma once

#include <cstddef>

#include "HttpRequest.hpp"

/**
 * @brief Incremental interface for parsing HTTP requests from a byte stream.
 *
 * Implementations receive raw TCP chunks through feed(), buffer incomplete data
 * internally, and expose completed requests through hasRequest() and
 * nextRequest(). Packet boundaries are not part of the contract.
 *
 * ```
 * parser.feed(data, size);
 * while (parser.hasRequest()) {
 *   HttpRequest* request = parser.nextRequest();
 *   // process request
 *   delete request;
 * }
 * ```
 */
class IHttpParser {
 public:
  /**
   * @brief Releases parser resources.
   */
  virtual ~IHttpParser();

  /**
   * @brief Feeds raw bytes from the TCP stream into the parser.
   *
   * @param data Input buffer.
   * @param size Number of bytes in @p data.
   *
   * Packet boundaries are not part of the parser contract. Implementations may
   * buffer partial input until a complete request is available.
   */
  virtual void feed(const char* data, size_t size) = 0;

  /**
   * @brief Returns true when a full request is ready to be popped.
   *
   * @return true when a request is available; false otherwise.
   */
  virtual bool hasRequest() const = 0;

  /**
   * @brief Returns the oldest completed request.
   *
   * The caller owns the returned object and must delete it when finished.
   * Returns NULL when no completed request is available.
   *
   * @return Oldest completed request, or NULL when none is available.
   */
  virtual HttpRequest* nextRequest() = 0;
};
