
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
#include <variant>

enum class Kind
{
        Int,
        Double,
};
struct Base
{
        explicit Base(Kind k) :kind{ k } {}
        virtual ~Base() {}
        Kind kind;
};
struct Int : Base
{
        Int(size_t x) : Base{ Kind::Int }, value { x } {}
        size_t value;
};
struct Double : Base
{
        Double(double x) : Base{ Kind::Double }, value{ x } {}
        double value;
};


static std::vector<std::shared_ptr<Base> > makePolyVector(size_t sz)
{
        std::vector<std::shared_ptr<Base>> result(0);
        result.reserve(sz);
        for (size_t idx = 0; idx != sz; ++idx)
        {
                if (idx % 2 == 0)
                {
                        result.push_back(std::make_shared<Int>(idx));
                }
                else
                {
                        result.push_back(std::make_shared<Double>(static_cast<double>(idx)));
                }
        }
        return result;
}

static std::vector<std::shared_ptr<Base> > makePolyPmrSharedVector(std::pmr::memory_resource* resource, size_t sz)
{
        std::pmr::polymorphic_allocator<Int> intAlloc{ resource };
        std::pmr::polymorphic_allocator<Double> doubleAlloc{ resource };
        std::vector<std::shared_ptr<Base>> result(0);
        result.reserve(sz);
        for (size_t idx = 0; idx != sz; ++idx)
        {
                if (idx % 2 == 0)
                {
                        result.push_back(std::allocate_shared<Int>(intAlloc, idx));
                }
                else
                {
                        result.push_back(std::allocate_shared<Double>(doubleAlloc, static_cast<double>(idx)));
                }
        }
        return result;
}
static std::vector<Base*> makePolyPmrRawVector(std::pmr::memory_resource* resource, size_t sz)
{
        std::pmr::polymorphic_allocator alloc{ resource };
        std::vector<Base*> result(0);
        result.reserve(sz);
        for (size_t idx = 0; idx != sz; ++idx)
        {
                if (idx % 2 == 0)
                {
                        result.push_back(alloc.new_object<Int>(idx));
                }
                else
                {
                        result.push_back(alloc.new_object<Double>(static_cast<double>(idx)));
                }
        }
        return result;
}


static std::vector<std::variant<Int,Double> > makeVariantVector(size_t sz)
{
        std::vector<std::variant<Int, Double> > result;
        result.reserve(sz);
        for (size_t idx = 0; idx != sz; ++idx)
        {
                if (idx % 2 == 0)
                {
                        result.push_back(Int(idx));
                }
                else
                {
                        result.push_back(Double(static_cast<double>(idx)));
                }
        }
        return result;
}

static void VariantVectorVisit(benchmark::State& state) {
        const auto sz = state.range(0);

        for (auto _ : state) {
                state.PauseTiming();
                auto V = makeVariantVector(sz);
                state.ResumeTiming();
                double sum = 0.0;
                for (const auto& v : V)
                {
                        sum += std::visit([](auto&& arg) -> double {return static_cast<double>(arg.value); }, v);
                }
                benchmark::DoNotOptimize(sum);
        }
}
BENCHMARK(VariantVectorVisit)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);


static void makePolyPmrSharedVector(benchmark::State& state) {
        const auto sz = state.range(0);

        constexpr size_t buffer_size = 1024 * 1024 * 16;
        std::vector<std::byte> buf(buffer_size);

                

        for (auto _ : state) {
                state.PauseTiming();
                std::pmr::monotonic_buffer_resource resource{ buf.data(), buf.size(), std::pmr::null_memory_resource() };
                auto V = makePolyPmrSharedVector(&resource, sz);
                state.ResumeTiming();
                double sum = 0.0;
                for (const auto& ptr : V)
                {
                        if (auto intPtr = dynamic_cast<Int*>(ptr.get()))
                        {
                                sum += intPtr->value;
                        }
                        else if (auto doublePtr = dynamic_cast<Double*>(ptr.get()))
                        {
                                sum += static_cast<int>(doublePtr->value);
                        }
                        else
                        {
                                throw std::domain_error("somethign bad");
                        }
                }
                benchmark::DoNotOptimize(sum);
        }
}
BENCHMARK(makePolyPmrSharedVector)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

static void makePolyPmrRawVector(benchmark::State& state) {
        const auto sz = state.range(0);

        constexpr size_t buffer_size = 1024 * 1024 * 16;
        std::vector<std::byte> buf(buffer_size);



        for (auto _ : state) {
                state.PauseTiming();
                std::pmr::monotonic_buffer_resource resource{ buf.data(), buf.size(), std::pmr::null_memory_resource() };
                auto V = makePolyPmrRawVector(&resource, sz);
                state.ResumeTiming();
                double sum = 0.0;
                for (const auto& ptr : V)
                {
                        if (auto intPtr = dynamic_cast<Int*>(ptr))
                        {
                                sum += intPtr->value;
                        }
                        else if (auto doublePtr = dynamic_cast<Double*>(ptr))
                        {
                                sum += static_cast<int>(doublePtr->value);
                        }
                        else
                        {
                                throw std::domain_error("somethign bad");
                        }
                }
                benchmark::DoNotOptimize(sum);
        }
}
BENCHMARK(makePolyPmrRawVector)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);

static void makePolyPmrRawVectorTypeSwitch(benchmark::State& state) {
        const auto sz = state.range(0);

        constexpr size_t buffer_size = 1024 * 1024 * 16;
        std::vector<std::byte> buf(buffer_size);



        for (auto _ : state) {
                state.PauseTiming();
                std::pmr::monotonic_buffer_resource resource{ buf.data(), buf.size(), std::pmr::null_memory_resource() };
                auto V = makePolyPmrRawVector(&resource, sz);
                state.ResumeTiming();
                double sum = 0.0;
                for (Base* ptr : V)
                {
                        switch (ptr->kind)
                        {
                        case Kind::Int:
                                sum += reinterpret_cast<Int*>(ptr)->value;
                                break;
                        case Kind::Double:
                                sum += reinterpret_cast<Double*>(ptr)->value;
                                break;
                        default:
                                throw std::domain_error("somethign bad");
                        }
                }
                benchmark::DoNotOptimize(sum);
        }
}
BENCHMARK(makePolyPmrRawVectorTypeSwitch)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);


static void PolyVector(benchmark::State& state) {
        const auto sz = state.range(0);

        for (auto _ : state) {
                state.PauseTiming();
                auto V = makePolyVector(sz);
                state.ResumeTiming();
                double sum = 0.0;
                for (const auto& ptr : V)
                {
                        if (auto intPtr = dynamic_cast<Int*>(ptr.get()))
                        {
                                sum += intPtr->value;
                        }
                        else if (auto doublePtr = dynamic_cast<Double*>(ptr.get()))
                        {
                                sum += static_cast<int>(doublePtr->value);
                        }
                        else
                        {
                                throw std::domain_error("somethign bad");
                        }
                }
                benchmark::DoNotOptimize(sum);
        }
}
BENCHMARK(PolyVector)->RangeMultiplier(2)->Range(2<<5,2<<10);

static void PolyVectorTypeSwitch(benchmark::State& state) {
        const auto sz = state.range(0);

        for (auto _ : state) {
                state.PauseTiming();
                auto V = makePolyVector(sz);
                state.ResumeTiming();
                double sum = 0.0;
                for (const std::shared_ptr<Base>& ptr : V)
                {
                        switch (ptr->kind)
                        {
                        case Kind::Int:
                                sum += reinterpret_cast<Int*>(ptr.get())->value;
                                break;
                        case Kind::Double:
                                sum += reinterpret_cast<Double*>(ptr.get())->value;
                                break;
                        default:
                                throw std::domain_error("somethign bad");
                        }
                }
                benchmark::DoNotOptimize(sum);
        }
}
BENCHMARK(PolyVectorTypeSwitch)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);



static void VariantVector(benchmark::State& state) {
        const auto sz = state.range(0);

        for (auto _ : state) {
                state.PauseTiming();
                auto V = makeVariantVector(sz);
                state.ResumeTiming();
                double sum = 0.0;
                for (const auto& v : V)
                {
                        if (auto intPtr = std::get_if<Int>(&v))
                        {
                                sum += intPtr->value;
                        }
                        else if (auto doublePtr = std::get_if<Double>(&v))
                        {
                                sum += static_cast<int>(doublePtr->value);
                        }
                        else
                        {
                                throw std::domain_error("somethign bad");
                        }
                }
                benchmark::DoNotOptimize(sum);
        }
}
BENCHMARK(VariantVector)->RangeMultiplier(2)->Range(2 << 5, 2 << 10);







BENCHMARK_MAIN();

