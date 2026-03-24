#pragma once

#include <string>

std::string buildHttp10Request(const std::string& requestLine,
                               const std::string& headers = std::string(),
                               const std::string& body = std::string());
