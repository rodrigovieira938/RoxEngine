#pragma once
namespace RoxEngine {
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
}