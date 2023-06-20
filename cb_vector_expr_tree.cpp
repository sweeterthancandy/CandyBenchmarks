
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

template<class... VectorType>
__declspec(noinline) void vectorAdd(
        std::vector<double>& out,
        const VectorType&... V)
{
        for (size_t idx = 0; idx != out.size(); ++idx)
        {
                out[idx] = (... + V[idx]);
        }
}

template<class... VectorType>
__declspec(noinline) void vectorAddAccumulate(
        std::vector<double>& out,
        const VectorType&... V)
{
        for (size_t idx = 0; idx != out.size(); ++idx)
        {
                out[idx] += (... + V[idx]);
        }
}

template<class T>
struct is_vector_field : std::false_type {};

template<class T>
constexpr bool is_vector_field_v = is_vector_field<T>::value;

struct StdVectorView
{
        explicit StdVectorView(const std::vector<double>& V) :V_{ &V } {}
        void Collect(std::vector<const std::vector<double>*>& T)const
        {
                T.push_back(V_);
        }
private:
        const std::vector<double>* V_;
};
template<>
struct is_vector_field< StdVectorView > : std::true_type {};



template<class LP, class RP>
struct VectorBinaryOp
{
        std::vector<double> Evaluate()const
        {
                std::vector<const std::vector<double>*> T;
                Collect(T);

                std::vector<double> out(T[0]->size());

                [&]()
                {
                        for (;;)
                        {
                                const auto S = T.size();
                                switch (T.size())
                                {
                                case 1:
                                        vectorAddAccumulate(out, *T[0]);
                                        return;
                                case 2:
                                        vectorAddAccumulate(out, *T[0], *T[1]);
                                        return;
                                case 3:
                                        vectorAddAccumulate(out, *T[0], *T[1], *T[2]);
                                        return;
                                case 4:
                                        vectorAddAccumulate(out, *T[S - 4], *T[S - 3], *T[S - 2], *T[S - 1]);
                                        return;
                                default:
                                        vectorAddAccumulate(out, *T[S - 4], *T[S - 3], *T[S - 2], *T[S - 1]);
                                        T.resize(S - 4);
                                        break;
                                }
                        }
                }();
                
                return out;
                
        }
        void Collect(std::vector<const std::vector<double>*>& T)const
        {
                lp_.Collect(T);
                rp_.Collect(T);
        }
        LP lp_;
        RP rp_;
};
template<class LP, class RP>
struct is_vector_field< VectorBinaryOp<LP,RP> > : std::true_type {};

template<class T>
struct is_vector_binary_op : std::false_type {};
template<class LP, class RP>
struct is_vector_binary_op< VectorBinaryOp< LP, RP> > : std::true_type {};
template<class T>
constexpr bool is_vector_binary_op_v = is_vector_binary_op<T>::value;

template<class LP, class RP>
auto MakeVectorBinaryOp(LP&& lp, RP&& rp)
{
        return VectorBinaryOp<LP,RP>(std::forward<LP>(lp), std::forward<RP>(rp));
};


template<class T>
constexpr bool always_false_v = false;

template<class T>
auto MakeView(T&& value)
{
        using value_ty = std::decay_t<T>;

        if constexpr (std::is_same_v < value_ty, std::vector<double> >)
        {
                return StdVectorView(value);
        }
        else if constexpr (std::is_same_v < value_ty, StdVectorView>)
        {
                return value;
        }
        else if constexpr (is_vector_binary_op_v < value_ty>)
        {
                return value;
        }
        else
        {
                static_assert(always_false_v<value_ty> );
        }
}

template<
        class LP,
        class RP,
        class = std::enable_if_t<
                is_vector_field_v<std::decay_t<LP>> ||
                is_vector_field_v<std::decay_t<RP>> > >
auto operator+(LP&& lp, RP&& rp)
{
        return MakeVectorBinaryOp(
                MakeView(lp),
                MakeView(rp));
}











template<class Vector, std::size_t... idx>
auto vector_sum_impl(Vector& V, std::index_sequence<idx...>)noexcept
{
        return (V[idx] + ...);
}

template<int N, class Vector>
auto vector_sum(Vector& V)noexcept
{
        return vector_sum_impl(V, std::make_index_sequence<N>{});
}


#include <optional>
#include <iostream>


static void ExprTreeBM(benchmark::State& state) {
        const auto N = static_cast<int>(state.range(0));
        const auto size = static_cast<int>(state.range(1));

        std::vector<std::vector<double>> VV;
        for (size_t idx = 0; idx != 16; ++idx)
        {
                VV.emplace_back(std::vector<double>(size, 2.0));
        }
        std::vector<StdVectorView> views;
        for (size_t idx = 0; idx != 16; ++idx)
        {
                views.push_back(MakeView(VV[idx]));
        }
        

        //std::optional<double> first;
        for (auto _ : state) {

                const auto sum = [&]() {
                        switch (N) {
                        case 2: return vector_sum<2>(views).Evaluate();
                        case 3: return vector_sum<3>(views).Evaluate();
                        case 4: return vector_sum<4>(views).Evaluate();
                        case 5: return vector_sum<5>(views).Evaluate();
                        case 6: return vector_sum<6>(views).Evaluate();
                        case 7: return vector_sum<7>(views).Evaluate();
                        case 8: return vector_sum<8>(views).Evaluate();
                        case 9: return vector_sum<9>(views).Evaluate();
                        case 10: return vector_sum<10>(views).Evaluate();
                        default: std::exit(0);
                        }
                }();
                //first = sum[0];
                benchmark::DoNotOptimize(sum);
        }
        //std::cout << first.value() << "\n";

        const auto double_sum = (MakeView(VV[0]) + VV[1]).Evaluate();


}
constexpr size_t VectorSize = 1024 * 64; // something we would use for monte carlo
BENCHMARK(ExprTreeBM)->Unit(benchmark::kMillisecond)->ArgsProduct({ {2,3,4,5,6,7,8,9,10},{VectorSize,VectorSize * 4,VectorSize * 16} });


BENCHMARK_MAIN();

