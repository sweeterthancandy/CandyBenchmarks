
#include <benchmark/benchmark.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <iterator>


// demonstrate how readings N points inside a vector of M
// depends on the denseness of reads
static void VectorIntRead(benchmark::State& state) {
        // count, stride
        const auto stride = state.range(0);
        const auto n = state.range(1);
        
        std::vector<size_t> V(n * stride);
        for (size_t idx = 0; idx != state.range(0); ++idx)
        {
                V[idx] = (idx* ((2 << 12) + 1)) % 3;
        }
        for (auto _ : state) {
                for (int counter =0; counter != 1000; ++counter)
                {
                        size_t total = 0;
                        for (size_t idx = 0; idx < V.size(); idx += stride)
                        {
                                total += V[idx];
                        }
                        benchmark::DoNotOptimize(total);
                }
                
        }
}
BENCHMARK(VectorIntRead)
->ArgsProduct({ benchmark::CreateRange(1, 1024*16, 2), {1024 * 32, 1024 * 64} });

BENCHMARK_MAIN();

