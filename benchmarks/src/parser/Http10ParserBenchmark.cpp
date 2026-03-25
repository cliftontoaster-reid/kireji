#include <benchmark/benchmark.h>

#include <cassert>
#include <cstdint>
#include <string>

#include "parser/Http10Parser.hpp"

static std::string makeHttp10Request(const std::string& requestLine,
                                     const std::string& headers = std::string(),
                                     const std::string& body = std::string()) {
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

#ifdef KIREJI_ENABLE_HTTP1_0

static void BM_Http10Parser_MinimalRequest(benchmark::State& state) {
  const std::string requestText = makeHttp10Request("GET / HTTP/1.0");

  for (auto _ : state) {
    Http10Parser parser;
    parser.feed(requestText.c_str(), requestText.size());

    assert(parser.hasRequest());
    HttpRequest* request = parser.nextRequest();
    assert(request != NULL);

    benchmark::DoNotOptimize(request->method);
    benchmark::DoNotOptimize(request->path);
    benchmark::DoNotOptimize(request->headers.size());

    delete request;
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(requestText.size()));
}
BENCHMARK(BM_Http10Parser_MinimalRequest);

static void BM_Http10Parser_FragmentedRequest(benchmark::State& state) {
  const std::string firstChunk =
      "POST /upload HTTP/1.0\r\nContent-Length: 5\r\n\r\nhe";
  const std::string secondChunk = "llo";

  for (auto _ : state) {
    Http10Parser parser;
    parser.feed(firstChunk.c_str(), firstChunk.size());
    parser.feed(secondChunk.c_str(), secondChunk.size());

    assert(parser.hasRequest());
    HttpRequest* request = parser.nextRequest();
    assert(request != NULL);

    benchmark::DoNotOptimize(request->method);
    benchmark::DoNotOptimize(request->path);
    benchmark::DoNotOptimize(request->body.is_some());

    delete request;
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
  state.SetBytesProcessed(
      static_cast<int64_t>(state.iterations()) *
      static_cast<int64_t>(firstChunk.size() + secondChunk.size()));
}
BENCHMARK(BM_Http10Parser_FragmentedRequest);

static void BM_Http10Parser_QueuedRequests(benchmark::State& state) {
  const std::string requestText = makeHttp10Request("GET /one HTTP/1.0") +
                                  makeHttp10Request("GET /two HTTP/1.0");

  for (auto _ : state) {
    Http10Parser parser;
    parser.feed(requestText.c_str(), requestText.size());

    assert(parser.hasRequest());
    HttpRequest* first = parser.nextRequest();
    assert(first != NULL);
    benchmark::DoNotOptimize(first->path);
    delete first;

    assert(parser.hasRequest());
    HttpRequest* second = parser.nextRequest();
    assert(second != NULL);
    benchmark::DoNotOptimize(second->path);
    delete second;
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * 2);
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(requestText.size()));
}
BENCHMARK(BM_Http10Parser_QueuedRequests);

#endif
