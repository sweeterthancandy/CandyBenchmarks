
#include <benchmark/benchmark.h>

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <iterator>


namespace
{
        std::string string_proto = "The quick brown fox jumped over the red gate 98yh308ghfa98ghdghs89ghs98ghfd90uvzjkjh546876574";
        using vector_ty = std::vector<std::shared_ptr<std::string> >;
        vector_ty make_source_vector()
        {
                constexpr size_t vector_size = 365*10;
                vector_ty result;
                for (size_t idx = 0; idx != vector_size; ++idx)
                {
                        result.push_back(std::make_shared<std::string>(string_proto));
                }
                return result;
        }


} // end namespace anon

static void Copy_CopyOnConstruct(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        state.ResumeTiming();

        vector_ty dest = src;

    }
}
BENCHMARK(Copy_CopyOnConstruct);

static void Copy_Assign(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        state.ResumeTiming();
        vector_ty dest;

        dest = src;

    }
}
BENCHMARK(Copy_Assign);

static void Copy_AssignZero(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        state.ResumeTiming();
        vector_ty dest(0);

        dest = src;

    }
}
BENCHMARK(Copy_AssignZero);



static void Copy_CopyAlgorithm(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        
        state.ResumeTiming();
        vector_ty dest;


        std::copy(src.begin(),src.end(),std::back_inserter(dest));

    }
}
BENCHMARK(Copy_CopyAlgorithm);


static void Copy_CopyAlgorithmReserve(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        state.ResumeTiming();

        vector_ty dest;
        dest.reserve(src.size());
        std::copy(src.begin(),src.end(),std::back_inserter(dest));

    }
}
BENCHMARK(Copy_CopyAlgorithmReserve);


static void Move_CopyWithMoveIterReserve(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        state.ResumeTiming();

        vector_ty dest;
        dest.reserve(src.size());
        std::copy(
                std::make_move_iterator(src.begin()),
                std::make_move_iterator(src.end()),
                std::back_inserter(dest));

    }
}
BENCHMARK(Move_CopyWithMoveIterReserve);

static void Move_CopyWithMoveIterReserveZero(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        state.ResumeTiming();

        vector_ty dest(0);
        dest.reserve(src.size());
        std::copy(
                std::make_move_iterator(src.begin()),
                std::make_move_iterator(src.end()),
                std::back_inserter(dest));

    }
}
BENCHMARK(Move_CopyWithMoveIterReserveZero);

static void Move_MoveAlgorithm(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        state.ResumeTiming();
        vector_ty dest;
        dest.resize(src.size());
        std::move(src.begin(),src.end(),dest.begin());

    }
}
BENCHMARK(Move_MoveAlgorithm);

static void Move_MoveAlgorithmZero(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        state.ResumeTiming();
        vector_ty dest(0);
        dest.resize(src.size());
        std::move(src.begin(),src.end(),dest.begin());

    }
}
BENCHMARK(Move_MoveAlgorithmZero);

static void Move_MoveAlgorithmInitSize(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        vector_ty src = make_source_vector();
        state.ResumeTiming();
        vector_ty dest(src.size());
        std::move(src.begin(),src.end(),dest.begin());

    }
}
BENCHMARK(Move_MoveAlgorithmInitSize);

BENCHMARK_MAIN();

