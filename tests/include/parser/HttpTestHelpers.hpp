#pragma once

#include <string>

/**
 * @brief Builds a minimal HTTP/1.0 request string for tests.
 *
 * @param requestLine Request line to place at the start of the request.
 * @param headers Optional header block, without the trailing blank line.
 * @param body Optional body payload.
 * @return A complete HTTP/1.0 request string.
 */
std::string buildHttp10Request(const std::string& requestLine,
                               const std::string& headers = std::string(),
                               const std::string& body = std::string());
