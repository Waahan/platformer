#pragma once
#include <memory>

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
} //Estd
