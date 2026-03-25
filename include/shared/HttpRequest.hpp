#pragma once

#include <ostream>
#include <stdexcept>
#include <string>

#include "HeaderMap.hpp"
#include "Option.hpp"

/**
 * @brief HTTP protocol versions supported by the library.
 *
 * Versions are rendered as standard HTTP tokens such as "HTTP/1.0".
 */
enum HttpVersion {
  HTTP_1_0 = 0,  //!< HTTP/1.0.
  HTTP_1_1 = 1,  //!< HTTP/1.1.
  HTTP_2_0 = 2,  //!< HTTP/2.0.
  HTTP_3_0 = 3,  //!< HTTP/3.0.
};

/**
 * @brief Writes an HTTP version token to @p os.
 *
 * @param os Output stream to write to.
 * @param version HTTP version to render.
 *
 * @return @p os after writing the version string.
 */
std::ostream& operator<<(std::ostream& os, HttpVersion version);

/**
 * @brief Returns the textual HTTP version token for @p version.
 *
 * @param version HTTP version to convert.
 * @return Textual HTTP version token.
 */
std::string to_string(HttpVersion version);

/**
 * @brief HTTP request methods recognized by the library.
 */
enum HttpMethod {
  GET,      //!< HTTP GET.
  POST,     //!< HTTP POST.
  PUT,      //!< HTTP PUT.
  DELETE,   //!< HTTP DELETE.
  HEAD,     //!< HTTP HEAD.
  OPTIONS,  //!< HTTP OPTIONS.
  PATCH,    //!< HTTP PATCH.
};

/**
 * @brief Parses @p method according to the rules for @p version.
 *
 * The method token is case-sensitive. Some methods are only valid for HTTP/1.1
 * and later.
 *
 * @param method Request method token.
 * @param version HTTP version used to validate the token.
 * @return The matching HttpMethod value.
 * @throws std::invalid_argument if the method is unknown or not allowed for
 * the given version.
 */
HttpMethod parseHttpMethod(const std::string& method, HttpVersion version);

/**
 * @brief Abstract HTTP request model shared by parsers and serializers.
 *
 * HttpRequest stores the request line, optional body, and headers as plain
 * fields. Concrete subclasses implement decoding and encoding for a specific
 * transport or HTTP version.
 *
 * ```c++
 * class ExampleRequest : public HttpRequest {
 *  public:
 *   ExampleRequest() : HttpRequest(GET, "/", HTTP_1_0) {}
 *   HttpRequest* decode(const char* data, size_t size) { return NULL; }
 *   std::string encode() const { return path; }
 * };
 * ```
 */
class HttpRequest {
 public:
  HttpMethod method;  //!< Request method from the request line.
  std::string path;  //!< Request target, typically the path portion of the URI.
  HttpVersion version;       //!< HTTP version from the request line.
  Option<std::string> body;  //!< Optional request body, or None when absent.
  HeaderMap headers;         //!< Parsed headers in insertion order.

  /**
   * @brief Builds a request without a body.
   *
   * @param m HTTP method.
   * @param p Request target path.
   * @param v HTTP version.
   */
  HttpRequest(HttpMethod m, const std::string& p, HttpVersion v);

  /**
   * @brief Builds a request with a body and headers.
   *
   * @param m HTTP method.
   * @param p Request target path.
   * @param v HTTP version.
   * @param b Request body.
   * @param h Headers to copy into the request.
   *
   * The headers are copied into the request.
   */
  HttpRequest(HttpMethod m, const std::string& p, HttpVersion v,
              const std::string& b, const HeaderMap& h);

  /**
   * @brief Builds a request with headers and no body.
   *
   * @param m HTTP method.
   * @param p Request target path.
   * @param v HTTP version.
   * @param h Headers to copy into the request.
   *
   * The headers are copied into the request.
   */
  HttpRequest(HttpMethod m, const std::string& p, HttpVersion v,
              const HeaderMap& h);

  /**
   * @brief Destroys the request.
   */
  virtual ~HttpRequest();

  /**
   * @brief Decodes a request from raw bytes.
   *
   * @param data Input buffer.
   * @param size Number of bytes in @p data.
   *
   * Implementations should return a heap-allocated request object or NULL when
   * the buffer does not yet contain a complete request.
   *
   * @return Heap-allocated request object, or NULL if incomplete.
   */
  virtual HttpRequest* decode(const char* data, size_t size) = 0;

  /**
   * @brief Serializes the request into HTTP wire format.
   *
   * @return Serialized HTTP request.
   */
  virtual std::string encode() const = 0;
};

/**
 * @brief Parses a complete HTTP request from raw bytes.
 *
 * @param data Input buffer.
 * @param size Number of bytes in @p data.
 *
 * Ownership of the returned request is transferred to the caller.
 *
 * @return Heap-allocated request object.
 */
HttpRequest* parseHttpRequest(const char* data, size_t size);