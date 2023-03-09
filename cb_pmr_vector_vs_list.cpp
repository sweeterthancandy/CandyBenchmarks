
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

constexpr const char prototype_long_text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

constexpr size_t min_strings = 2 << 8;
constexpr size_t max_strings = 2 << 12;

static void PmrVector(benchmark::State& state) {
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
BENCHMARK(PmrVector)->RangeMultiplier(2)->Range(min_strings, max_strings);

static void PmrList(benchmark::State& state) {
        const auto num_strings = state.range(0);


        constexpr size_t buffer_size = 1024 * 1024 * 16;
        std::vector<std::byte> buf(buffer_size);

        for (auto _ : state) {

                std::pmr::monotonic_buffer_resource pool{ buf.data(), buf.size(), std::pmr::null_memory_resource() };
                std::pmr::list<std::pmr::string> V{ 0, &pool };
                for (size_t idx = 0; idx != num_strings; ++idx)
                {
                        V.emplace_back(prototype_long_text);
                }
        }
}
BENCHMARK(PmrList)->RangeMultiplier(2)->Range(min_strings, max_strings);


BENCHMARK_MAIN();

