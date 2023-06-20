
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




static void AccumulateVector(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);



        for (auto _ : state) {

                const double result = std::reduce(
                        src.begin(), src.end(),
                        0.0);
                benchmark::DoNotOptimize(result);
        }

        state.SetBytesProcessed(int64_t(state.iterations()) *
                int64_t(state.range(0)));
}

BENCHMARK(AccumulateVector)->ArgsProduct({ benchmark::CreateRange(1024, 1024 << 20, 2) });

BENCHMARK_MAIN();

