
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



struct SingleRegister {
        template<auto offset>
        void Store(double value)noexcept {
                acc += value;
        }
        double Result()noexcept {
                return acc;
        }
        double acc{ 0 };
};
template<auto N>
struct MultiRegister {
        template<auto offset>
        void Store(double value)noexcept {
                acc[offset] += value;
        }
        double Result()noexcept {
                auto impl = [this](auto&& seq) {
                        return (... + acc[seq]);
                };
                return impl(std::make_index_sequence<N>{});
        }
        std::array<double, N> acc;
};

enum strategy {
        single_pass_single_reg,
        single_pass_multi_reg,
        multi_pass_single_reg,
        multi_pass_multi_reg,
};

template<strategy S, size_t N, class Iter>
double accumulate(Iter first, Iter last)noexcept{
        if constexpr (N == 1) {
                double result = 0.0;
                for (; first != last; first += 4) {
                        result += *(first);
                        result += *(first+1);
                        result += *(first+2);
                        result += *(first+3);
                }
                return result;
        }
        else if constexpr (N == 2) {
                double reg0 = 0.0;
                double reg1 = 0.0;
                for (; first != last; first += 4) {
                        reg0 += *first;
                        reg1 += *(first + 1);
                        reg0 += *(first + 2);
                        reg1 += *(first + 3);
                }
                return reg0 + reg1;
        }
        else if constexpr (N == 4) {
                double reg0 = 0.0;
                double reg1 = 0.0;
                double reg2 = 0.0;
                double reg3 = 0.0;
                for (; first != last; first += 4) {
                        reg0 += *first;
                        reg1 += *(first + 1);
                        reg2 += *(first + 2);
                        reg3 += *(first + 3);
                }
                return reg0 + reg1 + reg2 + reg3;
        }
        else if constexpr (N == 8) {
                double reg0 = 0.0;
                double reg1 = 0.0;
                double reg2 = 0.0;
                double reg3 = 0.0;
                double reg4 = 0.0;
                double reg5 = 0.0;
                double reg6 = 0.0;
                double reg7 = 0.0;
                for (; first != last; first += 8) {
                        reg0 += *first;
                        reg1 += *(first + 1);
                        reg2 += *(first + 2);
                        reg3 += *(first + 3);
                        reg4 += *(first + 4);
                        reg5 += *(first + 5);
                        reg6 += *(first + 6);
                        reg7 += *(first + 7);
                }
                return reg0 + reg1 + reg2 + reg3 + reg4 + reg5 + reg6 + reg7;
        }
        return -1;
}


static void VectorSumBaseline(benchmark::State& state) {

        const size_t num_reg = static_cast<size_t>(state.range(0));
        const auto n = static_cast<int>(state.range(1));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        double outer_result = 666;
        if (num_reg == 0) {
                for (auto _ : state) {

                        const double result = std::accumulate(src.begin(), src.end(), 0.0);
                        outer_result = result;
                        benchmark::DoNotOptimize(result);
                }
        }
        if (num_reg == 1) {
                for (auto _ : state) {

                        const double result = accumulate<1>(src.begin(), src.end());
                        outer_result = result;
                        benchmark::DoNotOptimize(result);
                }
        }
        if (num_reg == 2) {
                for (auto _ : state) {

                        const double result = accumulate<2>(src.begin(), src.end());
                        outer_result = result;
                        benchmark::DoNotOptimize(result);
                }
        }
        if (num_reg == 4) {
                for (auto _ : state) {

                        const double result = accumulate<4>(src.begin(), src.end());
                        outer_result = result;
                        benchmark::DoNotOptimize(result);
                }
        }
        if (num_reg == 8) {
                for (auto _ : state) {

                        const double result = accumulate<8>(src.begin(), src.end());
                        outer_result = result;
                        benchmark::DoNotOptimize(result);
                }
        }

        //std::cout << "result = " << outer_result << "\n";
       

        state.SetBytesProcessed(int64_t(state.iterations()) *
                int64_t(state.range(1)));
}

BENCHMARK(VectorSumBaseline)->ArgsProduct({ {0, 1,2,4,8 }, benchmark::CreateRange(1024 << 7, 1024 << 10, 2) });

BENCHMARK_MAIN();

