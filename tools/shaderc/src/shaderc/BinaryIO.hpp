// BinaryIO.hpp
//
// Small binary reader/writer. Reads and writes are sequential by default,
// but both sides also support tell()/seek() - needed for formats (like
// MaterialBinary.hpp's) that write a table of offsets before the data
// those offsets point to exists yet: write a placeholder, remember the
// position, write the data, come back and patch the placeholder once the
// real offset is known.

#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

class BinaryWriter
{
public:
    explicit BinaryWriter(std::ostream& stream)
        : mStream(stream)
    {
    }

    bool good() const { return mStream.good(); }

    void writeU8(uint8_t v) { mStream.write(reinterpret_cast<const char*>(&v), sizeof(v)); }
    void writeU32(uint32_t v) { mStream.write(reinterpret_cast<const char*>(&v), sizeof(v)); }
    void writeU64(uint64_t v) { mStream.write(reinterpret_cast<const char*>(&v), sizeof(v)); }
    void writeBool(bool v) { writeU8(v ? 1 : 0); }

    void writeBytes(const void* data, size_t size)
    {
        if (size == 0) return;
        mStream.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
    }

    void writeBytes(const std::vector<uint8_t>& bytes)
    {
        writeU32(static_cast<uint32_t>(bytes.size()));
        writeBytes(bytes.data(), bytes.size());
    }

    // Length-prefixed UTF-8 string (no null terminator).
    void writeString(const std::string& s)
    {
        writeU32(static_cast<uint32_t>(s.size()));
        writeBytes(s.data(), s.size());
    }

    uint64_t tell() { return static_cast<uint64_t>(mStream.tellp()); }

    void seek(uint64_t pos) { mStream.seekp(static_cast<std::streamoff>(pos)); }

private:
    std::ostream& mStream;
};

class BinaryReader
{
public:
    explicit BinaryReader(std::istream& stream)
        : mStream(stream)
    {
    }

    bool readU8(uint8_t& out)
    {
        mStream.read(reinterpret_cast<char*>(&out), sizeof(out));
        return static_cast<bool>(mStream);
    }

    bool readU32(uint32_t& out)
    {
        mStream.read(reinterpret_cast<char*>(&out), sizeof(out));
        return static_cast<bool>(mStream);
    }

    bool readU64(uint64_t& out)
    {
        mStream.read(reinterpret_cast<char*>(&out), sizeof(out));
        return static_cast<bool>(mStream);
    }

    bool readBool(bool& out)
    {
        uint8_t v = 0;
        if (!readU8(v)) return false;
        out = (v != 0);
        return true;
    }

    bool readBytes(void* dest, size_t size)
    {
        if (size == 0) return true;
        mStream.read(static_cast<char*>(dest), static_cast<std::streamsize>(size));
        return static_cast<bool>(mStream);
    }

    bool readBytes(std::vector<uint8_t>& out)
    {
        uint32_t size = 0;
        if (!readU32(size)) return false;
        out.resize(size);
        return readBytes(out.data(), size);
    }

    bool readString(std::string& out)
    {
        uint32_t len = 0;
        if (!readU32(len)) return false;
        out.resize(len);
        return readBytes(out.data(), len);
    }

    // Jump directly to an absolute byte offset - this is what makes the
    void seek(uint64_t pos) { mStream.seekg(static_cast<std::streamoff>(pos)); }
    uint64_t tell() { return static_cast<uint64_t>(mStream.tellg()); }

private:
    std::istream& mStream;
};