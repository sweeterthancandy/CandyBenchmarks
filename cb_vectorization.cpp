
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

__declspec(noinline) void vectorAdd(
        std::vector<double>& out,
        const std::vector<double>& A,
        const std::vector<double>& B)
{
        for (size_t idx = 0; idx != A.size(); ++idx)
        {
                out[idx] = A[idx] + B[idx];
        }
}

__declspec(noinline) void vectorMul(
        std::vector<double>& out,
        const std::vector<double>& A,
        const std::vector<double>& B)
{
        for (size_t idx = 0; idx != A.size(); ++idx)
        {
                out[idx] = A[idx] * B[idx];
        }
}

__declspec(noinline) void vectorLog(
        std::vector<double>& out,
        const std::vector<double>& X)
{
        for (size_t idx = 0; idx != out.size(); ++idx)
        {
                out[idx] = std::log(X[idx]);
        }
}

__declspec(noinline) void vectorInplaceAdd(
        std::vector<double>& LP,
        const std::vector<double>& RP)
{
        for (size_t idx = 0; idx != LP.size(); ++idx)
        {
                LP[idx] += RP[idx];
        }
}

__declspec(noinline) void vectorInplaceMul(
        std::vector<double>& LP,
        const std::vector<double>& RP)
{
        for (size_t idx = 0; idx != LP.size(); ++idx)
        {
                LP[idx] *= RP[idx];
        }
}

__declspec(noinline) void vectorInplaceLog(
        std::vector<double>& out)
{
        for (size_t idx = 0; idx != out.size(); ++idx)
        {
                out[idx] = std::log(out[idx]);
        }
}




__declspec(noinline) void vectorMullAdd(
        std::vector<double>& out,
        const std::vector<double>& A,
        const std::vector<double>& B,
        const std::vector<double>& C)
{
        for (size_t idx = 0; idx != A.size(); ++idx)
        {
                out[idx] = A[idx] * B[idx] + C[idx];
        }
}

__declspec(noinline) void vectorLogMulAdd(
        std::vector<double>& out,
        const std::vector<double>& A,
        const std::vector<double>& B,
        const std::vector<double>& C)
{
        for (size_t idx = 0; idx != A.size(); ++idx)
        {
                out[idx] = std::log(A[idx] * B[idx] + C[idx]);
        }
}

__declspec(noinline) void vectorMullAddMullAdd(
        std::vector<double>& out,
        const std::vector<double>& A,
        const std::vector<double>& B,
        const std::vector<double>& C,
        const std::vector<double>& D,
        const std::vector<double>& E)
{
        for (size_t idx = 0; idx != A.size(); ++idx)
        {
                out[idx] = (A[idx] * B[idx] + C[idx]) * D[idx] + E[idx];
        }
}





static void VectorAdd(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size);
        std::vector<double> B(size);

        for (auto _ : state) {
                
                vectorAdd(out, A, B);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(VectorAdd)->Arg(64000);

static void VectorMul(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size);
        std::vector<double> B(size);

        for (auto _ : state) {

                vectorMul(out, A, B);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(VectorMul)->Arg(64000);

static void VectorLog(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size, 2.0);

        for (auto _ : state) {

                vectorLog(out, A);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(VectorLog)->Arg(64000);




static void VectorInplaceAdd(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size);
        std::vector<double> B(size);

        for (auto _ : state) {

                vectorInplaceAdd(A, B);

                benchmark::DoNotOptimize(A);
        }
}
BENCHMARK(VectorInplaceAdd)->Arg(64000);


static void VectorInplaceMul(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size);
        std::vector<double> B(size);

        for (auto _ : state) {

                vectorInplaceMul(A, B);

                benchmark::DoNotOptimize(A);
        }
}
BENCHMARK(VectorInplaceMul)->Arg(64000);


static void VectorInplaceLog(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> X(size, 2.0);


        for (auto _ : state) {

                vectorInplaceLog(X);

                benchmark::DoNotOptimize(X);
        }
}
BENCHMARK(VectorInplaceLog)->Arg(64000);






static void VectorMulAdd(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size);
        std::vector<double> B(size);
        std::vector<double> C(size);

        for (auto _ : state) {

                vectorMullAdd(out, A, B, C);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(VectorMulAdd)->Arg(64000);

static void VectorMulAddSequenced(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size);
        std::vector<double> B(size);
        std::vector<double> C(size);

        for (auto _ : state) {

                vectorMul(out, A, B);
                vectorInplaceAdd(out, C);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(VectorMulAddSequenced)->Arg(64000);


static void VectorMulAddMullAdd(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size);
        std::vector<double> B(size);
        std::vector<double> C(size);
        std::vector<double> D(size);
        std::vector<double> E(size);

        for (auto _ : state) {

                vectorMullAddMullAdd(out, A, B, C, D, E);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(VectorMulAddMullAdd)->Arg(64000);


static void VectorMulAddMullAddSequenced(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size);
        std::vector<double> B(size);
        std::vector<double> C(size);
        std::vector<double> D(size);
        std::vector<double> E(size);

        for (auto _ : state) {

                vectorMul(out, A, B);
                vectorInplaceAdd(out, C);
                vectorInplaceMul(out, D);
                vectorInplaceAdd(out, E);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(VectorMulAddMullAddSequenced)->Arg(64000);





static void VectorLogMulAdd(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size);
        std::vector<double> B(size);
        std::vector<double> C(size);

        for (auto _ : state) {

                vectorLogMulAdd(out, A, B, C);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(VectorLogMulAdd)->Arg(64000);

static void VectorLogMulAddSequenced(benchmark::State& state) {
        const auto size = state.range(0);

        std::vector<double> out(size);

        std::vector<double> A(size, 2.0);
        std::vector<double> B(size, 2.0);
        std::vector<double> C(size, 2.0);

        for (auto _ : state) {

                vectorMul(out, A, B);
                vectorInplaceAdd(out, C);
                vectorInplaceLog(out);

                benchmark::DoNotOptimize(out);
        }
}
BENCHMARK(VectorLogMulAddSequenced)->Arg(64000);












BENCHMARK_MAIN();

