#pragma once
#include <string>
#include <memory>
#include <random>
#include <concepts>

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
        
        void operator()(T* resource) const noexcept { return destroyFunction(resource); } 
    };

    template<typename T, void (*destroyFunction)(T*)>
    using custom_unique_ptr = std::unique_ptr<T, customDelete<T, destroyFunction>>;

    template<typename T, void (*cleanUpFunction)(T)>
        requires std::copy_constructible<T>
    struct fileDescriptorHandle
    {
        static_assert(std::is_trivially_copyable_v<T> == true);
        public:
        fileDescriptorHandle() : fileDescriptor{} {}
        fileDescriptorHandle(T setFileDescriptor) : fileDescriptor{setFileDescriptor} {}

        fileDescriptorHandle(const fileDescriptorHandle& copyFrom) = delete;
        fileDescriptorHandle& operator=(const fileDescriptorHandle& copyFrom) = delete;

        fileDescriptorHandle(fileDescriptorHandle&& moveFrom)
        {
            fileDescriptor = moveFrom.fileDescriptor;

            moveFrom.fileDescriptor = 0;
        }

        fileDescriptorHandle& operator=(fileDescriptorHandle&& moveFrom)
        {
            fileDescriptor = moveFrom.fileDescriptor;

            moveFrom.fileDescriptor = 0;

            return *this;
        }

        ~fileDescriptorHandle() { if(fileDescriptor > 0) cleanUpFunction(fileDescriptor); }
        
        //Do not invoke cleanup function with this please
        explicit operator T() const { return fileDescriptor; } 

        T fileDescriptor;
    };

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
} //Estd
