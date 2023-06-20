
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





namespace detail {

        template<class T>
        struct SingleRegister {
                template<auto offset>
                constexpr void Store(T value)noexcept {
                        acc += value;
                }
                constexpr T Result()noexcept {
                        return acc;
                }
                T acc{ T{} };
        };
        template<class T, size_t N>
        struct MultiRegister {
                constexpr MultiRegister() {
                        acc.fill(T{});
                }
                template<size_t offset>
                constexpr void Store(T value)noexcept {
                        static_assert(offset < N, "");
                        acc[offset] += value;
                }
                template<size_t... Idx>
                constexpr T ResultImpl(std::index_sequence<Idx...>)noexcept {
                        return (... + acc[Idx]);
                }
                constexpr T Result()noexcept {
                        return ResultImpl(std::make_index_sequence<N>{});
                }
                std::array<T, N> acc;
        };

        template<class Register, class IterTuple, size_t... Idx>
        constexpr void stride_accumulate_impl_impl(Register& R, size_t sz, std::index_sequence<Idx...>, IterTuple iter)noexcept
        {
                for (int ttl = static_cast<int>(sz); ttl != 0;)
                {
                        --ttl;
                        (..., R.Store<Idx>(*std::get<Idx>(iter)));
                        (..., ++std::get<Idx>(iter));
                }
        }

        template<class Register, class Iter, std::size_t... idx>
        constexpr void stride_accumulate_impl(Register& R, size_t size, Iter first, std::index_sequence<idx...>)noexcept
        {
                size_t d = size / sizeof...(idx);
                auto iter_list = std::make_tuple((first + idx * d)...);
                stride_accumulate_impl_impl(R, size / sizeof...(idx), std::index_sequence<idx...>{}, iter_list);
        }

        template<size_t N, class Register, class Iter>
        constexpr void stride_accumulate(Register& R, Iter first, Iter last)noexcept
        {
                stride_accumulate_impl(R, std::distance(first,last), first, std::make_index_sequence<N>{});
        }

        template<size_t N, class Register, class Iter, std::size_t... idx>
        constexpr void single_accumulate_impl(Register& R, Iter iter, Iter last, std::index_sequence<idx...>)noexcept
        {
                for (; iter != last; iter += N) {
                        (..., R.Store<idx>(*(iter + idx)));
                }
        }
        template<size_t N, class Register, class Iter>
        constexpr void single_accumulate(Register& R, Iter first, Iter last)noexcept {
                single_accumulate_impl<N>(R, first, last, std::make_index_sequence<N>{});
        }

        namespace tests {
                constexpr bool test_single_register() {
                        SingleRegister<int> R;
                        R.Store<0>(1);
                        R.Store<1>(2);
                        if (R.Result() != 3) throw std::domain_error("unexpected");
                        return true;
                }
                static_assert(test_single_register());

                constexpr bool test_multi_register() {
                        MultiRegister<int, 2> R;
                        R.Store<0>(1);
                        R.Store<1>(2);
                        if (R.Result() != 3) throw std::domain_error("unexpected");
                        return true;
                }
                static_assert(test_multi_register());

                constexpr bool test_single_accumulate() {
                        SingleRegister<int> r1;
                        SingleRegister<int> r2;
                        MultiRegister<int, 2> m2;
                        std::vector<int> V{ 1,2,3, 4 };
                        single_accumulate<1>(r1, V.begin(), V.end());

                        if (r1.Result() != 10) throw std::domain_error("unexpected");

                        single_accumulate<2>(r2, V.begin(), V.end());
                        if (r2.Result() != 10) throw std::domain_error("unexpected");

                        single_accumulate<2>(m2, V.begin(), V.end());
                        if (m2.Result() != 10) throw std::domain_error("unexpected");

                        return true;
                }
                static_assert(test_single_accumulate());

                constexpr bool test_stride_accumulate() {
                        SingleRegister<int> r1;
                        SingleRegister<int> r2;
                        MultiRegister<int, 2> m2;
                        std::vector<int> V{ 1,2,3, 4 };
                        stride_accumulate<1>(r1, V.begin(), V.end());

                        if (r1.Result() != 10) throw std::domain_error("unexpected");

                        stride_accumulate<2>(r2, V.begin(), V.end());
                        if (r2.Result() != 10) throw std::domain_error("unexpected");

                        stride_accumulate<2>(m2, V.begin(), V.end());
                        if (m2.Result() != 10) throw std::domain_error("unexpected");

                        return true;
                }
                static_assert(test_stride_accumulate());

        } // namespace tests
} // namespace detail




enum strategy {
        single_pass_single_reg,
        single_pass_multi_reg,
        multi_pass_single_reg,
        multi_pass_multi_reg,
};

template<strategy S, size_t N, class Iter>
double accumulate(Iter first, Iter last)noexcept{
        if constexpr (S == strategy::single_pass_single_reg) {
                detail::SingleRegister<double> R;
                detail::single_accumulate<N>(R, first, last);
                return R.Result();
        }
        else if constexpr (S == strategy::single_pass_multi_reg) {
                detail::MultiRegister<double, N> R;
                detail::single_accumulate<N>(R, first, last);
                return R.Result();
        }
        else if constexpr (S == strategy::multi_pass_single_reg) {
                detail::SingleRegister<double> R;
                detail::stride_accumulate<N>(R, first, last);
                return R.Result();
        }
        else
        {
                detail::MultiRegister<double, N> R;
                detail::stride_accumulate<N>(R, first, last);
                return R.Result();
        }
}


template<strategy S>
double dispatch(size_t N, const std::vector<double>& V)noexcept {
        auto first = V.begin();
        auto last = V.end();
        switch (N) {
        case 1: return accumulate<S,1>(first, last);
        case 2: return accumulate<S,2>(first, last);
        case 4: return accumulate<S,4>(first, last);
        case 8: return accumulate<S,8>(first, last);
        case 16: return accumulate<S, 16>(first, last);
        default: std::exit(0);
        }
}
#include <iomanip>

template<strategy S>
static void VectorSumBaselineForS(benchmark::State& state) {

        const auto N = static_cast<size_t>(state.range(0));
        const auto vector_size = static_cast<int>(state.range(1));

        std::vector<double> V(vector_size);
        std::iota(V.begin(), V.end(), 0.0);

        if (N / 16 != 0) std::exit(0);


        double outer_result = 666;
        for (auto _ : state) {
                const auto result = [&]()->double {
                        switch (S) {
                        case strategy::single_pass_single_reg:
                                return dispatch< strategy::single_pass_single_reg>(N, V);
                        case strategy::single_pass_multi_reg:
                                return dispatch< strategy::single_pass_multi_reg>(N, V);
                        case strategy::multi_pass_single_reg:
                                return dispatch< strategy::multi_pass_single_reg>(N, V);
                        case strategy::multi_pass_multi_reg:
                                return dispatch< strategy::multi_pass_multi_reg>(N, V);
                        }
                        std::exit(0);
                }();
                outer_result = result;
                benchmark::DoNotOptimize(result);
        }


        //std::cout << std::setprecision(20) << "result = " << outer_result << "\n";


        state.SetBytesProcessed(int64_t(state.iterations()) *
                int64_t(vector_size));
}

static void BM_single_pass_single_reg(benchmark::State& state) {
        VectorSumBaselineForS< strategy::single_pass_single_reg>(state);
}
static void BM_single_pass_multi_reg(benchmark::State& state) {
        VectorSumBaselineForS< strategy::single_pass_multi_reg>(state);
}
static void BM_multi_pass_single_reg(benchmark::State& state) {
        VectorSumBaselineForS< strategy::multi_pass_single_reg>(state);
}
static void BM_multi_pass_multi_reg(benchmark::State& state) {
        VectorSumBaselineForS< strategy::multi_pass_multi_reg>(state);
}
BENCHMARK(BM_single_pass_single_reg)->ArgsProduct({  {1, 2,4, 8 }, benchmark::CreateRange(1024 << 10, 1024 << 20, 4) });
BENCHMARK(BM_single_pass_multi_reg)->ArgsProduct({  {1, 2,4, 8 }, benchmark::CreateRange(1024 << 10, 1024 << 20, 4) });
BENCHMARK(BM_multi_pass_single_reg)->ArgsProduct({  {1, 2,4, 8 }, benchmark::CreateRange(1024 << 10, 1024 << 20, 4) });
BENCHMARK(BM_multi_pass_multi_reg)->ArgsProduct({ {1, 2,4, 8 }, benchmark::CreateRange(1024 << 10, 1024 << 20, 4) });
#if 0
static void VectorSumBaseline(benchmark::State& state) {

        const auto S = static_cast<strategy>(state.range(0));
        const auto N = static_cast<size_t>(state.range(1));
        const auto vector_size = static_cast<int>(state.range(2));

        std::vector<double> V(vector_size);
        std::iota(V.begin(), V.end(), 0.0);

        if (N / 16 != 0) std::exit(0);


        double outer_result = 666;
        for (auto _ : state) {
                const auto result = [&]()->double {
                        switch (S) {
                        case strategy::single_pass_single_reg:
                                return dispatch< strategy::single_pass_single_reg>(N, V);
                        case strategy::single_pass_multi_reg:
                                return dispatch< strategy::single_pass_multi_reg>(N, V);
                        case strategy::multi_pass_single_reg:
                                return dispatch< strategy::multi_pass_single_reg>(N, V);
                        case strategy::multi_pass_multi_reg:
                                return dispatch< strategy::multi_pass_multi_reg>(N, V);
                        }
                        std::exit(0);
                }();
                outer_result = result;
                benchmark::DoNotOptimize(result);
        }
 

        //std::cout << std::setprecision(20) << "result = " << outer_result << "\n";
       

        state.SetBytesProcessed(int64_t(state.iterations()) *
                int64_t(vector_size));
}
BENCHMARK(VectorSumBaseline)->ArgsProduct({{0,1,2,3 }, {1, 2,4, 8 }, benchmark::CreateRange(1024 << 10, 1024 << 20, 4) });
#endif
BENCHMARK_MAIN();

