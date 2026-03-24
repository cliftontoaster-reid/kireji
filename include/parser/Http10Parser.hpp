#pragma once

#include "shared/IHttpParser.hpp"
#include "shared/Vec.hpp"

class Http10Parser : public IHttpParser {
 public:
  Http10Parser();
  ~Http10Parser() throw();

  void feed(const char* data, size_t size);

  bool hasRequest() const;
  HttpRequest* nextRequest();

 private:
  void drainBuffer();
  void clearRequests();

  std::string buffer_;
  Vec<HttpRequest*> requests_;

  Http10Parser(const Http10Parser& other);
  Http10Parser& operator=(const Http10Parser& other);
};
