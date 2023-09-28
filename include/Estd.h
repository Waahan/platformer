#pragma once
#include <string>
#include <memory>
#include <random>
#include <chrono>

namespace Estd
{
    template<typename T, void (*destroyFunction)(T*)>
    struct customDelete
    {
        public:
        customDelete() = default;

        customDelete(const customDelete& copyFrom) = default;
        customDelete& operator=(const customDelete& copyFrom) = default;

        customDelete(customDelete&& moveFrom) = default;
        customDelete& operator=(customDelete&& moveFrom) = default;

        ~customDelete() = default;
        
        void operator()(T* pointer) const noexcept { return destroyFunction(pointer); } 
    };

    template<typename T, void (*destroyFunction)(T*)>
    using custom_unique_ptr = std::unique_ptr<T, customDelete<T, destroyFunction>>;

    class randomNumberGenerator
    {
        public:
        static randomNumberGenerator& get();

        template<typename T, typename dist = std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>, std::uniform_int_distribution<T>>>
        T generate(T from, T to)
        {
            static_assert(std::is_arithmetic_v<T>, "generate requires either floating point or integar numbers");

            dist distribution{from, to};

            return distribution(ranGenerator); 
        }

        double entropy() const { return ranDevice.entropy(); }

        private:
        randomNumberGenerator() : ranDevice{}, ranGenerator{ranDevice()} {}

        randomNumberGenerator(const randomNumberGenerator& copyFrom) = delete;
        randomNumberGenerator& operator=(const randomNumberGenerator& copyFrom) = delete;

        randomNumberGenerator(randomNumberGenerator&& moveFrom) = delete;
        randomNumberGenerator& operator=(randomNumberGenerator&& moveFrom) = delete;

        ~randomNumberGenerator() = default;

        std::random_device ranDevice;
        std::mt19937 ranGenerator;
    };

    class timer
    {
        public:
        timer(const std::string& timerName = "Default") : start(std::chrono::steady_clock::now()), name{timerName} {}

        timer(const timer& copyFrom) = default;
        timer& operator=(const timer& copyFrom) = default;

        timer(timer&& moveFrom) = default;
        timer& operator=(timer&& moveFrom) = default;

        ~timer(); 

        void stop() const;

        private:
        std::chrono::time_point<std::chrono::steady_clock> start;
        std::string name;
    };
} //Estd
