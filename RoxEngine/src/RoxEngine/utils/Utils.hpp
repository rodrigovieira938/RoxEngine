#pragma once
#include <memory>
namespace RoxEngine {
#pragma region MEMORY_MANAGEMENT
    template<typename T>
        using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
        constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
        using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
        constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    struct Buffer {
        Buffer(size_t size) { mData = std::malloc(size); mSize = size; }

        void Free() { std::free(mData); mData = nullptr; }
        inline void* GetData() const { return mData; }
        inline size_t GetSize() const { return mSize; }
    private:
        void* mData;
        size_t mSize;
    };

    struct ScopedBuffer
    {
        ScopedBuffer(size_t size) : mBuffer(size) {}
        ScopedBuffer(Buffer&& buffer) : mBuffer(buffer) {}
        ~ScopedBuffer() { mBuffer.Free(); }
        inline void* GetData()  const { return mBuffer.GetData(); }
        inline size_t GetSize() const { return mBuffer.GetSize(); }

    private:
        Buffer mBuffer;
    };
#pragma endregion
}