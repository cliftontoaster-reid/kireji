#include <benchmark/benchmark.h>

#include "shared/Option.hpp"

static Option<int> optionAndThenIdentity(const int& value) {
  return Option<int>::Some(value);
}

static int fallbackValue() { return 123; }

static Option<int> fallbackOption() { return Option<int>::Some(456); }

static void BM_Option_ConstructNone(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value;
    benchmark::DoNotOptimize(value);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_ConstructNone);

static void BM_Option_ConstructSome(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(42);
    benchmark::DoNotOptimize(value);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_ConstructSome);

static void BM_Option_is_some_is_none(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> someValue = Option<int>::Some(42);
    Option<int> noneValue = Option<int>::None();
    bool a = someValue.is_some();
    bool b = someValue.is_none();
    bool c = noneValue.is_some();
    bool d = noneValue.is_none();
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * 4);
}
BENCHMARK(BM_Option_is_some_is_none);

static void BM_Option_isSome_isNone_aliases(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> someValue = Option<int>::Some(42);
    Option<int> noneValue = Option<int>::None();
    bool a = someValue.isSome();
    bool b = someValue.isNone();
    bool c = noneValue.isSome();
    bool d = noneValue.isNone();
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
    benchmark::DoNotOptimize(d);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * 4);
}
BENCHMARK(BM_Option_isSome_isNone_aliases);

static void BM_Option_is_some_and(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(42);
    bool result = value.is_some_and(42);
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_is_some_and);

static void BM_Option_is_none_or(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> valueNone = Option<int>::None();
    Option<int> valueSome = Option<int>::Some(42);
    bool a = valueNone.is_none_or(42);
    bool b = valueSome.is_none_or(42);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * 2);
}
BENCHMARK(BM_Option_is_none_or);

static void BM_Option_unwrap_or_some(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(42);
    int v = value.unwrap_or(84);
    benchmark::DoNotOptimize(v);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_unwrap_or_some);

static void BM_Option_unwrap_or_none(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::None();
    int v = value.unwrap_or(84);
    benchmark::DoNotOptimize(v);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_unwrap_or_none);

static void BM_Option_unwrap_or_else_some(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(42);
    int v = value.unwrap_or_else(fallbackValue);
    benchmark::DoNotOptimize(v);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_unwrap_or_else_some);

static void BM_Option_unwrap_or_else_none(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::None();
    int v = value.unwrap_or_else(fallbackValue);
    benchmark::DoNotOptimize(v);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_unwrap_or_else_none);

static void BM_Option_unwrap_or_default_some(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(42);
    int v = value.unwrap_or_default();
    benchmark::DoNotOptimize(v);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_unwrap_or_default_some);

static void BM_Option_unwrap_or_default_none(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::None();
    int v = value.unwrap_or_default();
    benchmark::DoNotOptimize(v);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_unwrap_or_default_none);

static void BM_Option_expect_some(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(42);
    int v = value.expect("should be there");
    benchmark::DoNotOptimize(v);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_expect_some);

static void BM_Option_take_some(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(42);
    Option<int> taken = value.take();
    benchmark::DoNotOptimize(taken);
    bool is_none = value.is_none();
    benchmark::DoNotOptimize(is_none);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_take_some);

static void BM_Option_replace_some(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(1);
    Option<int> previous = value.replace(2);
    benchmark::DoNotOptimize(previous);
    int unwrapped = value.unwrap();
    benchmark::DoNotOptimize(unwrapped);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_replace_some);

static void BM_Option_and_then_some(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(42);
    Option<int> result = value.and_then(optionAndThenIdentity);
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_and_then_some);

static void BM_Option_and_then_none(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::None();
    Option<int> result = value.and_then(optionAndThenIdentity);
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_and_then_none);

static void BM_Option_or_else_some(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::Some(42);
    Option<int> result = value.or_else(fallbackOption);
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_or_else_some);

static void BM_Option_or_else_none(benchmark::State& state) {
  for (auto _ : state) {
    Option<int> value = Option<int>::None();
    Option<int> result = value.or_else(fallbackOption);
    benchmark::DoNotOptimize(result);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()));
}
BENCHMARK(BM_Option_or_else_none);
