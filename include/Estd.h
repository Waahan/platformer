#pragma once
#include <string>
#include <memory>
#include <random>

#include "errors.h"

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

    template<typename T>
    class initGuard
    {
        public:
        initGuard(T destoryFunc) : destoryCallback{destoryFunc} {}

        initGuard(const initGuard& copyFrom) = delete;
        initGuard& operator=(const initGuard& copyFrom) = delete;

        initGuard(initGuard&& moveFrom) = default;
        initGuard& operator=(initGuard&& moveFrom) = default;

        ~initGuard() { destoryCallback(); }

        private:
        T destoryCallback;
    };

    class randomNumberGenerator
    {
        public:
        static randomNumberGenerator& get()
        {
            static randomNumberGenerator instance{};

            return instance;
        }

        template<typename T, typename dist = std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>, std::uniform_int_distribution<T>>>
        T generate(T from, T to)
        {
        /*
            Generate a random number from from to to

            Concept T is a number
            Precondition from is less than to
        */
            static_assert(std::is_arithmetic_v<T>, "generate requires either floating point or integar numbers");
            debug_assert((from < to), "From should be less than to");

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

    std::string generateUUID(int length = 20);
} //Estd
