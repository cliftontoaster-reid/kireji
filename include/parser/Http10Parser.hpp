#pragma once

#include "shared/IHttpParser.hpp"
#include "shared/Vec.hpp"

/**
 * @brief Incremental HTTP/1.0 parser that buffers input and queues requests.
 *
 * Http10Parser accepts raw TCP chunks, keeps incomplete data internally, and
 * exposes completed requests in arrival order. It is non-copyable.
 *
 * @code
 * Http10Parser parser;
 * parser.feed(data, size);
 * while (parser.hasRequest()) {
 *   HttpRequest* request = parser.nextRequest();
 *   // process request
 *   delete request;
 * }
 * @endcode
 */
class Http10Parser : public IHttpParser {
 public:
  /**
   * @brief Constructs an empty parser.
   */
  Http10Parser();

  /**
   * @brief Releases buffered state and any queued requests.
   */
  ~Http10Parser() throw();

  /**
   * @brief Feeds raw bytes into the parser.
   *
   * Packet boundaries are ignored. Completed requests are queued in FIFO
   * order, and incomplete tails are kept for the next call.
   *
   * @throws std::invalid_argument if the accumulated data contains a malformed
   * HTTP/1.0 request.
   */
  void feed(const char* data, size_t size);

  /**
   * @brief Returns true when at least one completed request is queued.
   */
  bool hasRequest() const;

  /**
   * @brief Returns the oldest completed request, or NULL when none is queued.
   *
   * The caller owns the returned object and must delete it.
   */
  HttpRequest* nextRequest();

 private:
  void drainBuffer();
  void clearRequests();

  std::string buffer_;
  Vec<HttpRequest*> requests_;

  Http10Parser(const Http10Parser& other);
  Http10Parser& operator=(const Http10Parser& other);
};
