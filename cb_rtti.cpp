
#include <benchmark/benchmark.h>

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <iterator>

struct Base
{
        virtual ~Base()=default;
        virtual bool IsA()const{ return false; }
};

struct A : Base{virtual bool IsA()const{ return true; }};
struct B : Base{};
struct C : Base{};


std::vector<Base*> make_random_vec()
{
        constexpr size_t size = 10000;
        std::vector<Base*> vec;
        for (size_t idx = 0; idx != size; ++idx)
        {
                vec.push_back(new A{});
                vec.push_back(new B{});
                vec.push_back(new C{});
        }
        std::random_shuffle(std::begin(vec),std::end(vec));
        return vec;
}
std::vector<std::shared_ptr<Base> > make_random_shared_vec()
{
        constexpr size_t size = 10000;
        std::vector<std::shared_ptr<Base> > vec;
        for (size_t idx = 0; idx != size; ++idx)
        {
                vec.push_back(std::make_shared<A>());
                vec.push_back(std::make_shared<B>());
                vec.push_back(std::make_shared<C>());
        }
        std::random_shuffle(std::begin(vec),std::end(vec));
        return vec;
}
static void NoRTTITest(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        const auto vec = make_random_vec();
        state.ResumeTiming();

        size_t counter = 0;
        for (auto const& x : vec)
        {
                if (x->IsA())
                {
                        ++counter;
                }
        }
        benchmark::DoNotOptimize(counter);
    }
}
BENCHMARK(NoRTTITest);
static void DynCastPtr(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        const auto vec = make_random_vec();
        state.ResumeTiming();

        size_t counter = 0;
        for (auto const& x : vec)
        {
                if (dynamic_cast<const A*>(x))
                {
                        ++counter;
                }
        }
        benchmark::DoNotOptimize(counter);
    }
}
BENCHMARK(DynCastPtr);
static void DynCastSharedPtr(benchmark::State& state) {
    for (auto _ : state)
    {
        state.PauseTiming();
        const auto vec = make_random_shared_vec();
        state.ResumeTiming();

        size_t counter = 0;
        for (auto const& x : vec)
        {
                if (std::dynamic_pointer_cast<const A>(x))
                {
                        ++counter;
                }
        }
        benchmark::DoNotOptimize(counter);
    }
}
BENCHMARK(DynCastSharedPtr);

BENCHMARK_MAIN();

