
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








static void VectorPushBack(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> V(0);
        V.reserve(n);

        for (auto _ : state) {
                state.PauseTiming();
                V.resize(0);
                state.ResumeTiming();
                for (int idx = 0; idx != n; ++idx)
                {
                        V.push_back(src[idx]);
                }
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();
        }
}
static void VectorAssign(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> V(n);

        for (auto _ : state) {
                for (int idx = 0; idx != n; ++idx)
                {
                        V[idx] = src[idx];
                }
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();
        }
}

static void RawVectorAssign(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> V(n);
        double* ptr = &*V.begin();

        for (auto _ : state) {
                for (int idx = 0; idx != n; ++idx)
                {
                        ptr[idx] = src[idx];
                }
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();
        }
}

static void VectorCopy(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> V(n);

        for (auto _ : state) {
                
                std::copy(src.cbegin(), src.cend(), V.begin());
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();
        }
}

static void VectorRawCopy(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> V(n);

        for (auto _ : state) {

                const auto src_start = &*src.cbegin();
                std::copy(src_start, src_start+src.size(), &*V.begin());
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();
        }
}

static void VectorCopyStride(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));
        const auto m = static_cast<int>(state.range(1));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> V(n);



        for (auto _ : state) {

                auto out = V.begin();
                for (auto iter = src.cbegin(), end = src.cend(); iter + m < end; iter+=m)
                {
                        std::copy(iter, iter+m, out);
                        out += m;
                }
                
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();
        }
}

static void VectorSelfInsert(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> V(n);

        for (auto _ : state) {

                V.resize(0);
                V.insert(V.begin(), src.begin(), src.end());
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();
        }
}

static void VectorSelfAssign(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        std::vector<double> V(n);

        for (auto _ : state) {

                V.assign(src.begin(), src.end());
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();
        }
}

enum class QuickVectorStorageLayout
{
        START_END,
        START_OFFSET,
};
template<class T, QuickVectorStorageLayout layout>
class QuickVector
{
public:
        constexpr explicit QuickVector(size_t size)noexcept
        {
                ptr_ = new double[size];
                if constexpr (layout == QuickVectorStorageLayout::START_END)
                {
                        end_ = ptr_;
                }
                else
                {
                        end_ = 0;
                }
        }
        constexpr ~QuickVector()
        {
                delete[] ptr_;
        }
        constexpr void push_back(T value)noexcept
        {
                if constexpr (layout == QuickVectorStorageLayout::START_END)
                {
                        *end_ = value;
                        ++end_;
                }
                else
                {
                        ptr_[end_] = value;
                        ++end_;
                }
        }
        constexpr size_t size()const noexcept
        {
                if constexpr (layout == QuickVectorStorageLayout::START_END)
                {
                        return end_ - ptr_;
                }
                else
                {
                        return end_;
                }
        }
        constexpr T operator[](size_t index)const noexcept
        {
                return ptr_[index];
        }
        constexpr T& operator[](size_t index)noexcept
        {
                return ptr_[index];
        }
private:
        double* ptr_;
        std::conditional_t<
                layout == QuickVectorStorageLayout::START_END,
                double*,
                size_t> end_;
};

namespace QuickVectorTests
{
        template<QuickVectorStorageLayout layout>
        constexpr bool test0()
        {
                QuickVector<double, layout> V(10);
                if (V.size() != 0) return false;
                V.push_back(0);
                if (V.size() != 1) return false;
                V.push_back(1);
                V.push_back(2);

                if (V[0] != 0) return false;
                if (V[1] != 1) return false;
                if (V[2] != 2) return false;

                if (V.size() != 3) return false;
                return true;

        }

        static_assert(test0<QuickVectorStorageLayout::START_END>());
        static_assert(test0<QuickVectorStorageLayout::START_OFFSET>());

} // namespace QuickVectorTests



static void QuickVectorStartEnd(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        QuickVector<double, QuickVectorStorageLayout::START_END> V(n);

        for (auto _ : state) {

                for (int idx = 0; idx != n; ++idx)
                {
                        V[idx] = src[idx];
                }
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();

        }
}

static void QuickVectorStartOffset(benchmark::State& state) {

        const auto n = static_cast<int>(state.range(0));

        std::vector<double> src(n);
        std::iota(src.begin(), src.end(), 0.0);

        QuickVector<double, QuickVectorStorageLayout::START_OFFSET> V(n);

        for (auto _ : state) {

                for (int idx = 0; idx != n; ++idx)
                {
                        V[idx] = src[idx];
                }
                benchmark::DoNotOptimize(V);
                benchmark::ClobberMemory();
        }
}


constexpr size_t NumSteps = 1024 * 1024 * 64;
BENCHMARK(VectorPushBack)->Arg(NumSteps);
BENCHMARK(VectorAssign)->Arg(NumSteps);
BENCHMARK(VectorCopy)->Arg(NumSteps);
BENCHMARK(VectorCopyStride)->ArgsProduct({ {NumSteps}, {1,2,4,8,16,32,64,128,256,1024*8} });
BENCHMARK(VectorSelfInsert)->Arg(NumSteps);
BENCHMARK(VectorSelfAssign)->Arg(NumSteps);

#if 0

using int_type = int;

#pragma optimize("",off)
void increment(int_type& x, int_type& y)noexcept
{
        ++x;
        ++y;
}
void transform_twice(std::vector<int_type>& V)
{
        ++V[0];
        ++V[0];
}
void transform_next(std::vector<int_type>& V)
{
        ++V[0];
        ++V[1];
}
void transform_twice_noexcept(std::vector<int_type>& V)noexcept
{
        ++V[0];
        ++V[0];
}
void transform_next_noexcept(std::vector<int_type>& V)noexcept
{
        ++V[0];
        ++V[1];
}
#pragma optimize("",on)

static void BMIncTwice(benchmark::State& state) {

        const auto steps = static_cast<int>(state.range(0));

        for (auto _ : state) {
                int_type first = 0;
                int_type second = 0;
                for (int idx = 0; idx != steps; ++idx)
                {
                        increment(first, first);
                }
                benchmark::DoNotOptimize(first);
                benchmark::DoNotOptimize(second);
        }
}
static void BMIncNext(benchmark::State& state) {

        const auto steps = static_cast<int>(state.range(0));

        for (auto _ : state) {
                int_type first = 0;
                int_type second = 0;
                for (int idx = 0; idx != steps; ++idx)
                {
                        increment(first, second);
                }
                benchmark::DoNotOptimize(first);
                benchmark::DoNotOptimize(second);
        }
}


constexpr size_t NumSteps = 1024 * 1024 * 64;
BENCHMARK(BMIncTwice)->Arg(NumSteps);
BENCHMARK(BMIncNext)->Arg(NumSteps);

#endif


#if 0

#pragma optimize("",off)
int compute_sum(const int_type* first, const int_type* last, int stride)
{
        int result = 0;
        for (; first < last; first += stride)
        {
                result += *first;
        }
        return result;
}
#pragma optimize("",on)


static void cacheRead(benchmark::State& state) {

        const auto stride = static_cast<int>(state.range(0));
        const auto size = static_cast<int>(state.range(1));

        

        for (auto _ : state) {
                state.PauseTiming();
                std::vector<int_type> V(size);
                state.ResumeTiming();
                int sum = compute_sum(&V[0], &V[0] + size, stride);
                benchmark::DoNotOptimize(sum);
        }
}

constexpr size_t VectorSize = 1024 * 1024 * 64 / 8; // something we would use for monte carlo
BENCHMARK(cacheRead)->ArgsProduct({ benchmark::CreateDenseRange(1, 2, 1),{VectorSize} });
BENCHMARK(cacheRead)->ArgsProduct({ benchmark::CreateDenseRange(1, 8, 1),{VectorSize} });
BENCHMARK(cacheRead)->ArgsProduct({ benchmark::CreateRange(32, 64*16, 2),{VectorSize} });

BENCHMARK(cacheRead)->ArgsProduct({ benchmark::CreateDenseRange(1, 2, 1),{VectorSize} });
BENCHMARK(cacheRead)->ArgsProduct({ benchmark::CreateDenseRange(1, 8, 1),{VectorSize} });
BENCHMARK(cacheRead)->ArgsProduct({ benchmark::CreateRange(32, 64 * 16, 2),{VectorSize} });

#endif

BENCHMARK_MAIN();

