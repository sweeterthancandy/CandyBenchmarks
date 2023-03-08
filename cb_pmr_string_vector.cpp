
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

constexpr const char prototype_long_text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

constexpr size_t min_strings = 2 << 8;
constexpr size_t max_strings = 2 << 10;

static void PureMemcopy(benchmark::State& state) {
        
        
        std::array<
                std::array<char, sizeof(prototype_long_text)>,
                max_strings
        > A; 
        
        for (auto _ : state) {
                for (size_t idx = 0; idx != max_strings; ++idx)
                {
                        benchmark::DoNotOptimize(std::memcpy(A[idx].data(), prototype_long_text, sizeof(prototype_long_text)));
                }
        }
}
BENCHMARK(PureMemcopy);


static void StdStringVector(benchmark::State& state) {
        const auto num_strings = state.range(0);

        for (auto _ : state) {
                std::vector<std::string> V;
                for (size_t idx = 0; idx != num_strings; ++idx)
                {
                        V.emplace_back(prototype_long_text);
                }
        }
}

BENCHMARK(StdStringVector)->RangeMultiplier(2)->Range(min_strings, max_strings);



static void PmrStringPmrVector_NoAllocator(benchmark::State& state) {
        const auto num_strings = state.range(0);

        for (auto _ : state) {
                std::pmr::vector<std::pmr::string> V;
                for (size_t idx = 0; idx != num_strings; ++idx)
                {
                        V.emplace_back(prototype_long_text);
                }
        }
}
BENCHMARK(PmrStringPmrVector_NoAllocator)->RangeMultiplier(2)->Range(min_strings, max_strings);


static void PmrStringPmrVector_MonotonicBuffer(benchmark::State& state) {
        const auto num_strings = state.range(0);

        for (auto _ : state) {
                
                std::pmr::monotonic_buffer_resource mbr;
                std::pmr::vector<std::pmr::string> V{ 0, &mbr };
                V.reserve(num_strings);
                for (size_t idx = 0; idx != num_strings; ++idx)
                {
                        V.emplace_back(prototype_long_text);
                }
        }
}
BENCHMARK(PmrStringPmrVector_MonotonicBuffer)->RangeMultiplier(2)->Range(min_strings, max_strings);


static void PmrStringPmrVector_MonotonicBuffer_PreAlloc(benchmark::State& state) {
        const auto num_strings = state.range(0);


        constexpr size_t buffer_size = 1024 * 1024 * 16;
        std::vector<std::byte> buf(buffer_size);

        for (auto _ : state) {

                std::pmr::monotonic_buffer_resource pool{ buf.data(), buf.size(), std::pmr::null_memory_resource() };
                std::pmr::vector<std::pmr::string> V{ 0, &pool };
                V.reserve(num_strings);
                for (size_t idx = 0; idx != num_strings; ++idx)
                {
                        V.emplace_back(prototype_long_text);
                }
        }
}
BENCHMARK(PmrStringPmrVector_MonotonicBuffer_PreAlloc)->RangeMultiplier(2)->Range(min_strings, max_strings);

static void PmrStringPmrVector_SynchronizedPoolResource(benchmark::State& state) {
        const auto num_strings = state.range(0);

        for (auto _ : state) {

                std::pmr::synchronized_pool_resource resource;
                std::pmr::vector<std::pmr::string> V{ 0, &resource };
                V.reserve(num_strings);
                for (size_t idx = 0; idx != num_strings; ++idx)
                {
                        V.emplace_back(prototype_long_text);
                }
        }
}
BENCHMARK(PmrStringPmrVector_SynchronizedPoolResource)->RangeMultiplier(2)->Range(min_strings, max_strings);

static void PmrStringPmrVector_UnsynchronizedPoolResource(benchmark::State& state) {
        const auto num_strings = state.range(0);

        for (auto _ : state) {

                std::pmr::unsynchronized_pool_resource resource;
                std::pmr::vector<std::pmr::string> V{ 0, &resource };
                V.reserve(num_strings);
                for (size_t idx = 0; idx != num_strings; ++idx)
                {
                        V.emplace_back(prototype_long_text);
                }
        }
}
BENCHMARK(PmrStringPmrVector_UnsynchronizedPoolResource)->RangeMultiplier(2)->Range(min_strings, max_strings);







BENCHMARK_MAIN();

