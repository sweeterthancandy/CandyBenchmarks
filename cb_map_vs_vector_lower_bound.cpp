
#include <benchmark/benchmark.h>

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <random>
#include <iterator>
#include <array>
#include <memory_resource>



template<size_t ValueSize>
std::tuple<
        std::vector<int64_t>,
        std::vector<std::pair<int64_t, std::array<std::byte, ValueSize> > >,
        std::map<int64_t, std::array<std::byte, ValueSize>>,
        std::pmr::map<int64_t, std::array<std::byte, ValueSize>>> makeMapAndIndex(int64_t size)
{
        constexpr size_t buffer_size = 1024 * 1024 * 16;
        static std::vector<std::byte> buf(buffer_size);
        static std::pmr::monotonic_buffer_resource pool{ buf.data(), buf.size(), std::pmr::null_memory_resource() };

        std::map<int64_t, std::array<std::byte, ValueSize>> M;
        std::pmr::map<int64_t, std::array<std::byte, ValueSize>> pmrM;
        std::vector < std::pair<int64_t, std::array<std::byte, ValueSize> > > V;
        std::vector<int64_t> index;

        std::array<std::byte, ValueSize> dummy_value;
        for (int64_t iter = 0; iter != size; ++iter)
        {
                M.emplace(iter, dummy_value);
                pmrM.emplace(iter, dummy_value);
                V.emplace_back(iter, dummy_value);
                index.push_back(iter);
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(index.begin(), index.end(), g);
        return std::make_tuple(std::move(index), std::move(V), std::move(M), std::move(pmrM));
}

static void StdVectorStdLowerBound(benchmark::State& state) {
        const auto size = state.range(0);

        auto [index, V, M, pmrM] = makeMapAndIndex<8>(size);

        for (auto _ : state) {
                for (auto i : index)
                {
                        benchmark::DoNotOptimize(std::lower_bound(V.begin(), V.end(), i, [](const auto& p, int value)->bool { return p.first < value; }));
                }
        }
}
BENCHMARK(StdVectorStdLowerBound)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

static void StdMapMemberLowerBound(benchmark::State& state) {
        const auto size = state.range(0);
        auto [index, V, M, pmrM] = makeMapAndIndex<8>(size);

        for (auto _ : state) {
                for (auto i : index)
                {
                        benchmark::DoNotOptimize(M.lower_bound(i));
                }
        }
}
BENCHMARK(StdMapMemberLowerBound)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

static void PmrMapMemberLowerBound(benchmark::State& state) {
        const auto size = state.range(0);
        auto [index, V, M, pmrM] = makeMapAndIndex<8>(size);

        for (auto _ : state) {
                for (auto i : index)
                {
                        benchmark::DoNotOptimize(pmrM.lower_bound(i));
                }
        }
}
BENCHMARK(PmrMapMemberLowerBound)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

static void StdMapStdLowerBound(benchmark::State& state) {
        const auto size = state.range(0);
        auto [index, V, M, pmrM] = makeMapAndIndex<8>(size);

        for (auto _ : state) {
                for (auto i : index)
                {
                        benchmark::DoNotOptimize(std::lower_bound(M.begin(), M.end(), i, [](const auto& p, int value)->bool { return p.first < value; }));
                }
        }
}
BENCHMARK(StdMapStdLowerBound)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

static void PmrMapStdLowerBound(benchmark::State& state) {
        const auto size = state.range(0);
        auto [index, V, M, pmrM] = makeMapAndIndex<8>(size);

        for (auto _ : state) {
                for (auto i : index)
                {
                        benchmark::DoNotOptimize(std::lower_bound(pmrM.begin(), pmrM.end(), i, [](const auto& p, int value)->bool { return p.first < value; }));
                }
        }
}
BENCHMARK(PmrMapStdLowerBound)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);




static void StdMapRead(benchmark::State& state) {
        const auto size = state.range(0);
        auto [index, V, M, pmrM] = makeMapAndIndex<8>(size);

        for (auto _ : state) {
                for (const auto& p : M)
                {
                        benchmark::DoNotOptimize(p.first);
                        benchmark::DoNotOptimize(p.second);
                }
        }
}
BENCHMARK(StdMapRead)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

static void PmrMapRead(benchmark::State& state) {
        const auto size = state.range(0);
        auto [index, V, M, pmrM] = makeMapAndIndex<8>(size);

        for (auto _ : state) {
                for (const auto& p : pmrM)
                {
                        benchmark::DoNotOptimize(p.first);
                        benchmark::DoNotOptimize(p.second);
                }
        }
}
BENCHMARK(PmrMapRead)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

static void StdMapReadFresh(benchmark::State& state) {
        const auto size = state.range(0);


        for (auto _ : state) {
                state.PauseTiming();
                auto [index, V, M, pmrM] = makeMapAndIndex<8>(size);
                state.ResumeTiming();
                for (const auto& p : M)
                {
                        benchmark::DoNotOptimize(p.first);
                        benchmark::DoNotOptimize(p.second);
                }
        }
}
BENCHMARK(StdMapReadFresh)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

static void PmrMapReadFresh(benchmark::State& state) {
        const auto size = state.range(0);


        for (auto _ : state) {
                state.PauseTiming();
                auto [index, V, M, pmrM] = makeMapAndIndex<8>(size);
                state.ResumeTiming();
                for (const auto& p : pmrM)
                {
                        benchmark::DoNotOptimize(p.first);
                        benchmark::DoNotOptimize(p.second);
                }
        }
}
BENCHMARK(PmrMapReadFresh)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

BENCHMARK_MAIN();

