#include <benchmark/benchmark.h>

#include <string>

#include "shared/Vec.hpp"

static void BM_Vec_ConstructEmpty(benchmark::State& state) {
  for (auto _ : state) {
    Vec<int> v;
    benchmark::DoNotOptimize(v);
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Vec_ConstructEmpty);

static void BM_Vec_PushBack(benchmark::State& state) {
  const int count = 1000;

  for (auto _ : state) {
    Vec<int> v;
    for (int i = 0; i < count; ++i) {
      v.push_back(i);
      benchmark::ClobberMemory();
    }
    benchmark::DoNotOptimize(v.size());
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * count);
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * count *
                          sizeof(int));
}
BENCHMARK(BM_Vec_PushBack);

static void BM_Vec_PushBack_WithReserve(benchmark::State& state) {
  const int count = 1000;

  for (auto _ : state) {
    Vec<int> v;
    v.reserve(count);
    for (int i = 0; i < count; ++i) {
      v.push_back(i);
    }
    benchmark::DoNotOptimize(v.size());
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * count);
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * count *
                          sizeof(int));
}
BENCHMARK(BM_Vec_PushBack_WithReserve);

static void BM_Vec_Reserve(benchmark::State& state) {
  const std::size_t reserveSize = 1000;

  for (auto _ : state) {
    Vec<int> v;
    v.reserve(reserveSize);
    benchmark::DoNotOptimize(v);
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Vec_Reserve);

static void BM_Vec_PopFront(benchmark::State& state) {
  const int count = 1000;

  for (auto _ : state) {
    Vec<int> v;
    for (int i = 0; i < count; ++i) {
      v.push_back(i);
    }
    for (int i = 0; i < count; ++i) {
      int value = v.pop_front();
      benchmark::DoNotOptimize(value);
    }
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * count);
}
BENCHMARK(BM_Vec_PopFront);

static void BM_Vec_Clear(benchmark::State& state) {
  const int count = 1000;

  for (auto _ : state) {
    Vec<int> v;
    for (int i = 0; i < count; ++i) {
      v.push_back(i);
    }
    v.clear();
    benchmark::DoNotOptimize(v.size());
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * count);
}
BENCHMARK(BM_Vec_Clear);

static void BM_Vec_EmptyAndSize(benchmark::State& state) {
  for (auto _ : state) {
    Vec<int> v;
    bool empty = v.empty();
    std::size_t size = v.size();
    benchmark::DoNotOptimize(empty);
    benchmark::DoNotOptimize(size);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Vec_EmptyAndSize);

static void BM_Vec_FrontAccess(benchmark::State& state) {
  const int count = 1000;

  Vec<int> v;
  for (int i = 0; i < count; ++i) {
    v.push_back(i);
  }

  for (auto _ : state) {
    int first = v.front();
    benchmark::DoNotOptimize(first);
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Vec_FrontAccess);

static void BM_Vec_BracketAccess(benchmark::State& state) {
  const int count = 1000;

  Vec<int> v;
  for (int i = 0; i < count; ++i) {
    v.push_back(i);
  }

  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < count; ++i) {
      sum += v[i];
    }
    benchmark::DoNotOptimize(sum);
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * count);
}
BENCHMARK(BM_Vec_BracketAccess);

static void BM_Vec_CopyConstructor(benchmark::State& state) {
  const int count = 1000;

  Vec<int> source;
  for (int i = 0; i < count; ++i) {
    source.push_back(i);
  }

  for (auto _ : state) {
    Vec<int> copy(source);
    benchmark::DoNotOptimize(copy.size());
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Vec_CopyConstructor);

static void BM_Vec_AssignmentOperator(benchmark::State& state) {
  const int count = 1000;

  Vec<int> source;
  for (int i = 0; i < count; ++i) {
    source.push_back(i);
  }

  Vec<int> target;

  for (auto _ : state) {
    target = source;
    benchmark::DoNotOptimize(target.size());
  }

  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Vec_AssignmentOperator);
