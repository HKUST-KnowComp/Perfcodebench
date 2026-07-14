#include "interface.h"
#include "lz4.h"
#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1 << 20;   // 1 MB
constexpr int kAcceleration = 65537;           // maximum acceleration for fastest compression
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    chunks.clear();
    const std::size_t len = input.size();
    if (len == 0) return 0;
    const std::size_t num_chunks = (len + kChunkSize - 1) / kChunkSize;
    chunks.reserve(num_chunks);
    const char* src = input.data();
    int total = 0;
    for (std::size_t offset = 0; offset < len; offset += kChunkSize) {
        const std::size_t take = (offset + kChunkSize > len) ? (len - offset) : kChunkSize;
        const int srcSize = static_cast<int>(take);
        const int dstCapacity = LZ4_compressBound(srcSize);
        chunks.emplace_back();
        std::string& compressed = chunks.back();
        compressed.resize(static_cast<std::size_t>(dstCapacity));
        const int compressedSize = LZ4_compress_fast(src + offset, compressed.data(),
                                                     srcSize, dstCapacity, kAcceleration);
        if (compressedSize <= 0) {
            return -1;
        }
        compressed.resize(static_cast<std::size_t>(compressedSize));
        total += compressedSize;
    }
    return total;
}
