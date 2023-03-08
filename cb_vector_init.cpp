
#include <benchmark/benchmark.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <iterator>
#include <array>
#include <memory_resource>

constexpr size_t reserve_size = 1024 * 1024;

static void IntVector_Default(benchmark::State& state) {
        for (auto _ : state) {
                std::vector<int> V;
                benchmark::DoNotOptimize(V);
        }
}
BENCHMARK(IntVector_Default);

static void IntVector_Zero(benchmark::State& state) {
        for (auto _ : state) {
                std::vector<int> V(0);
                benchmark::DoNotOptimize(V);
        }
}
BENCHMARK(IntVector_Zero);

static void IntVector_Reserve(benchmark::State& state) {
        for (auto _ : state) {
                std::vector<int> V;
                V.reserve(reserve_size);
                benchmark::DoNotOptimize(V);
        }
}
BENCHMARK(IntVector_Reserve);

static void IntVector_Resize(benchmark::State& state) {
        for (auto _ : state) {
                std::vector<int> V;
                V.resize(reserve_size);
                benchmark::DoNotOptimize(V);
        }
}
BENCHMARK(IntVector_Resize);

static void IntVector_ZeroReserve(benchmark::State& state) {
        for (auto _ : state) {
                std::vector<int> V(0);
                V.reserve(reserve_size);
                benchmark::DoNotOptimize(V);
        }
}
BENCHMARK(IntVector_ZeroReserve);

static void IntVector_ZeroResize(benchmark::State& state) {
        for (auto _ : state) {
                std::vector<int> V(0);
                V.resize(reserve_size);
                benchmark::DoNotOptimize(V);
        }
}
BENCHMARK(IntVector_ZeroResize);

static void IntVector_InitSize(benchmark::State& state) {
        for (auto _ : state) {
                std::vector<int> V(reserve_size);
                benchmark::DoNotOptimize(V);
        }
}
BENCHMARK(IntVector_InitSize);

static void IntVector_Fill(benchmark::State& state) {
        
        for (auto _ : state) {
                
                state.PauseTiming();
                std::vector<int> V(reserve_size);
                state.ResumeTiming();
                std::fill(V.begin(), V.end(), 0.0);
                benchmark::DoNotOptimize(V);
        }
}
BENCHMARK(IntVector_Fill);

BENCHMARK_MAIN();

