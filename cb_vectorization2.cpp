
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
}

constexpr size_t VectorSize = 1024 * 64; // something we would use for monte carlo
BENCHMARK(vectorPlus)->Unit(benchmark::kMillisecond)->ArgsProduct({  {0,1,2,3,4,5,6,7,8,16,32,64,128},{VectorSize,VectorSize * 4,VectorSize * 16} });


#if 0

static void vectorStdAccumuate(benchmark::State& state) {
        
        const auto size = static_cast<int>(state.range(0));

        std::vector<double> out(size);

        std::vector<std::vector<double>> VV;
        for (size_t idx = 0; idx != 16; ++idx)
        {
                VV.emplace_back(std::vector<double>(size, 2.0));
        }

        for (auto _ : state) {

                vectorAdd(out, VV[0], VV[1], VV[2], VV[3], VV[4], VV[5], VV[6], VV[7], VV[8], VV[9], VV[10], VV[11], VV[12], VV[13], VV[14], VV[15]);
                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(vectorAdd16)->Unit(benchmark::kMillisecond)->ArgsProduct({
      benchmark::CreateRange(1024, 1024*256, /*multi=*/2)
        });


static void vectorAdd8x8(benchmark::State& state) {

        const auto size = static_cast<int>(state.range(0));

        std::vector<double> out(size);

        std::vector<std::vector<double>> VV;
        for (size_t idx = 0; idx != 16; ++idx)
        {
                VV.emplace_back(std::vector<double>(size, 2.0));
        }

        for (auto _ : state) {

                vectorAdd(out, VV[0], VV[1], VV[2], VV[3], VV[4], VV[5], VV[6], VV[7]);
                vectorAddAccumulate(out, VV[8], VV[9], VV[10], VV[11], VV[12], VV[13], VV[14], VV[15]);
                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(vectorAdd8x8)->Unit(benchmark::kMillisecond)->ArgsProduct({
      benchmark::CreateRange(1024, 1024 * 256, /*multi=*/2)
        });



static void vectorAdd4x4(benchmark::State& state) {

        const auto size = static_cast<int>(state.range(0));

        std::vector<double> out(size);

        std::vector<std::vector<double>> tmp;
        for (size_t idx = 0; idx != 4; ++idx)
        {
                tmp.emplace_back(std::vector<double>(size, 2.0));
        }

        std::vector<std::vector<double>> VV;
        for (size_t idx = 0; idx != 16; ++idx)
        {
                VV.emplace_back(std::vector<double>(size, 2.0));
        }

        for (auto _ : state) {

                vectorAdd(out, VV[0], VV[1], VV[2], VV[3]);
                vectorAddAccumulate(out, VV[4], VV[5], VV[6], VV[7]);
                vectorAddAccumulate(out, VV[8], VV[9], VV[10], VV[11]);
                vectorAddAccumulate(out, VV[12], VV[13], VV[14], VV[15]);
                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(vectorAdd4x4)->Unit(benchmark::kMillisecond)->ArgsProduct({
      benchmark::CreateRange(1024, 1024 * 256, /*multi=*/2)
        });

static void vectorAdd128(benchmark::State& state) {

        const auto size = static_cast<int>(state.range(0));

        std::vector<double> out(size);

        std::vector<std::vector<double>> VV;
        for (size_t idx = 0; idx != 128; ++idx)
        {
                VV.emplace_back(std::vector<double>(size, 2.0));
        }

        for (auto _ : state) {

                vectorAdd(out, VV[0], VV[1], VV[2], VV[3], VV[4], VV[5], VV[6], VV[7], VV[8], VV[9], VV[10], VV[11], VV[12], VV[13], VV[14], VV[15], VV[16], VV[17], VV[18], VV[19], VV[20], VV[21], VV[22], VV[23], VV[24], VV[25], VV[26], VV[27], VV[28], VV[29], VV[30], VV[31], VV[32], VV[33], VV[34], VV[35], VV[36], VV[37], VV[38], VV[39], VV[40], VV[41], VV[42], VV[43], VV[44], VV[45], VV[46], VV[47], VV[48], VV[49], VV[50], VV[51], VV[52], VV[53], VV[54], VV[55], VV[56], VV[57], VV[58], VV[59], VV[60], VV[61], VV[62], VV[63], VV[64], VV[65], VV[66], VV[67], VV[68], VV[69], VV[70], VV[71], VV[72], VV[73], VV[74], VV[75], VV[76], VV[77], VV[78], VV[79], VV[80], VV[81], VV[82], VV[83], VV[84], VV[85], VV[86], VV[87], VV[88], VV[89], VV[90], VV[91], VV[92], VV[93], VV[94], VV[95], VV[96], VV[97], VV[98], VV[99], VV[100], VV[101], VV[102], VV[103], VV[104], VV[105], VV[106], VV[107], VV[108], VV[109], VV[110], VV[111], VV[112], VV[113], VV[114], VV[115], VV[116], VV[117], VV[118], VV[119], VV[120], VV[121], VV[122], VV[123], VV[124], VV[125], VV[126], VV[127]);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(vectorAdd128)->Unit(benchmark::kMillisecond)->ArgsProduct({
      benchmark::CreateRange(1024, 1024 * 256, /*multi=*/2)
        });
static void vectorAdd2x64(benchmark::State& state) {

        const auto size = static_cast<int>(state.range(0));

        std::vector<double> out(size);

        std::vector<std::vector<double>> VV;
        for (size_t idx = 0; idx != 128; ++idx)
        {
                VV.emplace_back(std::vector<double>(size, 2.0));
        }

        for (auto _ : state) {

                vectorAdd(out, VV[0], VV[1], VV[2], VV[3], VV[4], VV[5], VV[6], VV[7], VV[8], VV[9], VV[10], VV[11], VV[12], VV[13], VV[14], VV[15], VV[16], VV[17], VV[18], VV[19], VV[20], VV[21], VV[22], VV[23], VV[24], VV[25], VV[26], VV[27], VV[28], VV[29], VV[30], VV[31], VV[32], VV[33], VV[34], VV[35], VV[36], VV[37], VV[38], VV[39], VV[40], VV[41], VV[42], VV[43], VV[44], VV[45], VV[46], VV[47], VV[48], VV[49], VV[50], VV[51], VV[52], VV[53], VV[54], VV[55], VV[56], VV[57], VV[58], VV[59], VV[60], VV[61], VV[62], VV[63]);
                vectorAddAccumulate(out, VV[64], VV[65], VV[66], VV[67], VV[68], VV[69], VV[70], VV[71], VV[72], VV[73], VV[74], VV[75], VV[76], VV[77], VV[78], VV[79], VV[80], VV[81], VV[82], VV[83], VV[84], VV[85], VV[86], VV[87], VV[88], VV[89], VV[90], VV[91], VV[92], VV[93], VV[94], VV[95], VV[96], VV[97], VV[98], VV[99], VV[100], VV[101], VV[102], VV[103], VV[104], VV[105], VV[106], VV[107], VV[108], VV[109], VV[110], VV[111], VV[112], VV[113], VV[114], VV[115], VV[116], VV[117], VV[118], VV[119], VV[120], VV[121], VV[122], VV[123], VV[124], VV[125], VV[126], VV[127]);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(vectorAdd2x64)->Unit(benchmark::kMillisecond)->ArgsProduct({
      benchmark::CreateRange(1024, 1024 * 256, /*multi=*/2)
        });

#endif

BENCHMARK_MAIN();

