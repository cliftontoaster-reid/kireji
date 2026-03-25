#include <benchmark/benchmark.h>

#include <string>

#include "shared/HeaderMap.hpp"

static const int kHeaderMapBenchmarkCount = 1000;

static void BM_HeaderMap_ConstructEmpty(benchmark::State& state) {
  for (auto _ : state) {
    HeaderMap map;
    benchmark::DoNotOptimize(map);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_HeaderMap_ConstructEmpty);

static void BM_HeaderMap_SetNew(benchmark::State& state) {
  for (auto _ : state) {
    HeaderMap map;
    for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
      map.set("X-Test-" + std::to_string(i), "value");
    }
    benchmark::DoNotOptimize(map.size());
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kHeaderMapBenchmarkCount);
}
BENCHMARK(BM_HeaderMap_SetNew);

static void BM_HeaderMap_SetExisting(benchmark::State& state) {
  HeaderMap map;
  for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
    map.set("X-Test-" + std::to_string(i), "value");
  }

  for (auto _ : state) {
    for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
      map.set("X-Test-" + std::to_string(i), "updated");
    }
    benchmark::DoNotOptimize(map.size());
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kHeaderMapBenchmarkCount);
}
BENCHMARK(BM_HeaderMap_SetExisting);

static void BM_HeaderMap_GetExisting(benchmark::State& state) {
  HeaderMap map;
  for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
    map.set("X-Test-" + std::to_string(i), "value");
  }

  for (auto _ : state) {
    for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
      const std::string* value = map.get("X-Test-" + std::to_string(i));
      benchmark::DoNotOptimize(value);
    }
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kHeaderMapBenchmarkCount);
}
BENCHMARK(BM_HeaderMap_GetExisting);

static void BM_HeaderMap_GetMissing(benchmark::State& state) {
  HeaderMap map;
  for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
    map.set("X-Test-" + std::to_string(i), "value");
  }

  for (auto _ : state) {
    for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
      const std::string* value = map.get("Missing-" + std::to_string(i));
      benchmark::DoNotOptimize(value);
    }
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kHeaderMapBenchmarkCount);
}
BENCHMARK(BM_HeaderMap_GetMissing);

static void BM_HeaderMap_RemoveExisting(benchmark::State& state) {
  for (auto _ : state) {
    HeaderMap map;
    for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
      map.set("X-Test-" + std::to_string(i), "value");
    }
    for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
      bool removed = map.remove("X-Test-" + std::to_string(i));
      benchmark::DoNotOptimize(removed);
    }
    benchmark::DoNotOptimize(map.size());
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kHeaderMapBenchmarkCount);
}
BENCHMARK(BM_HeaderMap_RemoveExisting);

static void BM_HeaderMap_RemoveMissing(benchmark::State& state) {
  HeaderMap map;
  for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
    map.set("X-Test-" + std::to_string(i), "value");
  }

  for (auto _ : state) {
    for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
      bool removed = map.remove("Missing-" + std::to_string(i));
      benchmark::DoNotOptimize(removed);
    }
    benchmark::DoNotOptimize(map.size());
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kHeaderMapBenchmarkCount);
}
BENCHMARK(BM_HeaderMap_RemoveMissing);

static void BM_HeaderMap_Clear(benchmark::State& state) {
  for (auto _ : state) {
    HeaderMap map;
    for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
      map.set("X-Test-" + std::to_string(i), "value");
    }
    map.clear();
    benchmark::DoNotOptimize(map.size());
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kHeaderMapBenchmarkCount);
}
BENCHMARK(BM_HeaderMap_Clear);

static void BM_HeaderMap_CopyConstructor(benchmark::State& state) {
  HeaderMap source;
  for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
    source.set("X-Test-" + std::to_string(i), "value");
  }

  for (auto _ : state) {
    HeaderMap copy(source);
    benchmark::DoNotOptimize(copy.size());
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_HeaderMap_CopyConstructor);

static void BM_HeaderMap_AssignmentOperator(benchmark::State& state) {
  HeaderMap source;
  for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
    source.set("X-Test-" + std::to_string(i), "value");
  }

  for (auto _ : state) {
    HeaderMap target;
    target = source;
    benchmark::DoNotOptimize(target.size());
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_HeaderMap_AssignmentOperator);

static void BM_HeaderMap_Iteration(benchmark::State& state) {
  HeaderMap map;
  for (int i = 0; i < kHeaderMapBenchmarkCount; ++i) {
    map.set("X-Test-" + std::to_string(i), "value");
  }

  for (auto _ : state) {
    std::size_t count = 0;
    for (HeaderMap::Node* node = map.head(); node; node = node->next) {
      benchmark::DoNotOptimize(node->name);
      benchmark::DoNotOptimize(node->value);
      ++count;
    }
    benchmark::DoNotOptimize(count);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * kHeaderMapBenchmarkCount);
}
BENCHMARK(BM_HeaderMap_Iteration);

static void BM_HeaderMap_CaseInsensitiveLookup(benchmark::State& state) {
  HeaderMap map;
  map.set("X-Test-Case", "value");

  for (auto _ : state) {
    const std::string* v1 = map.get("x-test-case");
    const std::string* v2 = map.get("X-TEST-CASE");
    benchmark::DoNotOptimize(v1);
    benchmark::DoNotOptimize(v2);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_HeaderMap_CaseInsensitiveLookup);
