
#include <benchmark/benchmark.h>

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <random>
#include <iterator>
#include <array>
#include <functional>
#include <numeric>
#include <memory_resource>


std::tuple<std::vector<double>, std::vector<std::vector<double>>> makeVectors(size_t size, size_t N) {
        std::vector<double> out(size);

        std::vector<std::vector<double>> VV;
        for (size_t idx = 0; idx != N; ++idx)
        {
                VV.emplace_back(std::vector<double>(size, 2.0));
        }
        return std::make_tuple(std::move(out), std::move(VV));
}



template<class... Iter>
double stride_accumulate_impl_impl(size_t sz, Iter... iter)noexcept
{
        double sum = 0.0;
        for (int ttl = static_cast<int>(sz); ttl != 0;)
        {
                --ttl;
                sum += (... + *iter);
                (++iter, ...);
        }
        return sum;
}

template<class Iter, std::size_t... idx>
double stride_accumulate_impl(size_t size, Iter first, std::index_sequence<idx...>)noexcept
{
        size_t d = size / sizeof...(idx);
        size_t res = size - sizeof...(idx) * d;
        const auto vectorizedSum = stride_accumulate_impl_impl(size / sizeof...(idx), (first + idx * d)...);
        const auto flatSum = std::accumulate(first + sizeof...(idx) * d, first + size,0.0);
        return vectorizedSum + flatSum;
}

template<int N, class Iter>
double stride_accumulate(size_t size, Iter first)noexcept
{
        return stride_accumulate_impl(size, first, std::make_index_sequence<N>{});
}


static void vectorPlus(benchmark::State& state) {

       
        const auto splits = static_cast<int>(state.range(0));
        const auto size = static_cast<int>(state.range(1));

        const auto [out, VV] = makeVectors(size, 1);
        const auto& V = VV[0];

        for (auto _ : state) {
                const auto iter = std::cbegin(V);
                const auto end = std::cend(V);
                const auto sum = [&]() {
                        switch (splits) {
                        case 0: return std::accumulate(iter, end, 0.0);
                        case 1: return stride_accumulate<1>(size, iter);
                        case 2: return stride_accumulate<2>(size, iter);
                        case 3: return stride_accumulate<3>(size, iter);
                        case 4: return stride_accumulate<4>(size, iter);
                        case 5: return stride_accumulate<5>(size, iter);
                        case 6: return stride_accumulate<6>(size, iter);
                        case 7: return stride_accumulate<7>(size, iter);
                        case 8: return stride_accumulate<8>(size, iter);
                        case 16: return stride_accumulate<16>(size, iter);
                        case 32: return stride_accumulate<32>(size, iter);
                        case 64: return stride_accumulate<64>(size, iter);
                        case 128: return stride_accumulate<128>(size, iter);
                        default: std::exit(0);
                        }
                }();
                benchmark::DoNotOptimize(sum);
        }

        state.SetItemsProcessed(state.iterations() * size);
}

constexpr size_t VectorSize = 1024 * 64; // something we would use for monte carlo
BENCHMARK(vectorPlus)->Unit(benchmark::kMillisecond)->ArgsProduct({  {0,1,2,3,4,5,6,7,8,16,32,64,128},{VectorSize,VectorSize * 4,VectorSize * 16} });



BENCHMARK_MAIN();

