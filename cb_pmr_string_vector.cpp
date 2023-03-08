
#include <benchmark/benchmark.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <iterator>
#include <memory_resource>

constexpr const char* prototype_long_text =  "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

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
BENCHMARK(StdStringVector)->RangeMultiplier(2)->Range(8, 2 << 10);

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
BENCHMARK(PmrStringPmrVector_NoAllocator)->RangeMultiplier(2)->Range(8, 2 << 10);


static void PmrStringPmrVector_MonotonicBuffer(benchmark::State& state) {
        const auto num_strings = state.range(0);

        std::vector<std::byte> buf(num_strings * sizeof(prototype_long_text) + 1);
        std::pmr::monotonic_buffer_resource pool{ buf.data(), buf.size() };

        for (auto _ : state) {
                std::pmr::vector<std::pmr::string> V{ &pool };
                for (size_t idx = 0; idx != num_strings; ++idx)
                {
                        V.push_back(std::pmr::string(prototype_long_text, &pool));
                }
        }
}
BENCHMARK(PmrStringPmrVector_MonotonicBuffer)->RangeMultiplier(2)->Range(8, 2 << 10);

static void Dummy(benchmark::State& state) {
        const auto num_strings = state.range(0);

        std::vector<std::byte> buf(num_strings * sizeof(prototype_long_text) + 1);
        std::pmr::monotonic_buffer_resource pool{ buf.data(), buf.size() };

        for (auto _ : state) {
        }
}
BENCHMARK(Dummy)->RangeMultiplier(2)->Range(8, 2 << 10);

BENCHMARK_MAIN();

