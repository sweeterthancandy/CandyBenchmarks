
#include <benchmark/benchmark.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <iterator>
#include <list>
#include <array>
#include <memory_resource>
#include <functional>
#include <numeric>


static void CreateVectorAssign(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);


        std::vector<double> result(n);

        for (auto _ : state) {
        
                for (size_t idx = 0; idx != n; ++idx)
                        result[idx] = src[idx] * 2;
                benchmark::DoNotOptimize(result);
        }
}

static void CreateVectorPushback(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> result(n);

        for (auto _ : state) {

                result.resize(0);
                for (size_t idx = 0; idx != n; ++idx)
                        result.push_back(src[idx] * 2);
                benchmark::DoNotOptimize(result);
        }
}

static void CreateVectorTransformPushback(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> result(n);

        for (auto _ : state) {

                result.resize(0);
                std::transform(
                        src.cbegin(), src.cend(),
                        std::back_inserter(result),
                        [](auto&& x) { return x * 2; });
                benchmark::DoNotOptimize(result);
        }
}

static void CreateVectorTransformAssign(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> result(n);

        for (auto _ : state) {
                std::transform(
                        src.cbegin(), src.cend(),
                        result.begin(),
                        [](auto&& x) { return x * 2; });
                benchmark::DoNotOptimize(result);
        }
}

static void CreateVectorTransformPtrAssign(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> result(n);

        for (auto _ : state) {


                std::transform(
                        src.cbegin(), src.cend(),
                        &result[0],
                        [](auto&& x) { return x * 2; });
                benchmark::DoNotOptimize(result);
        }
}

static void CreateVectorReducePushback(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);



        for (auto _ : state) {

                std::vector<double> result = std::reduce(
                        src.begin(), src.end(),
                        [size = n]() {
                                std::vector<double> V(0);
                                V.reserve(size);
                                return V;
                        }(),
                        [](auto&& V, auto&& x) {
                                V.push_back(x);
                                return V;
                        });
                benchmark::DoNotOptimize(result);
        }
}
static void CreateVectorReducePtr(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);



        for (auto _ : state) {

                std::vector<double> result = std::get<0>(std::reduce(
                        src.begin(), src.end(),
                        [size = n]() {
                                std::vector<double> V(size);
                                return std::make_tuple(std::move(V), &V.front());
                        }(),
                        [](auto&& V, auto&& x) {
                                *std::get<1>(V) = x;
                                ++std::get<1>(V);
                                return std::move(V);
                        }));
                benchmark::DoNotOptimize(result);
        }
}

BENCHMARK(CreateVectorAssign)->Range(1024, 1024 * 32);
BENCHMARK(CreateVectorPushback)->Range(1024, 1024 * 32);
BENCHMARK(CreateVectorTransformAssign)->Range(1024, 1024 * 32);
BENCHMARK(CreateVectorTransformPtrAssign)->Range(1024, 1024 * 32);
BENCHMARK(CreateVectorTransformPushback)->Range(1024, 1024 * 32);
BENCHMARK(CreateVectorReducePushback)->Range(1024, 1024 * 32);
BENCHMARK(CreateVectorReducePtr)->Range(1024, 1024 * 32);

BENCHMARK_MAIN();

