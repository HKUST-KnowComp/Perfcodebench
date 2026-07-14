#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    chunks.clear();
    if (input.empty()) return 0;
    
    const std::size_t num_chunks = (input.size() + kChunkSize - 1) / kChunkSize;
    chunks.reserve(num_chunks);
    
    const int full_chunk_bound = LZ4_compressBound(static_cast<int>(kChunkSize));
    
    int total = 0;
    const char* src = input.data();
    const std::size_t input_size = input.size();
    
    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t take = std::min(kChunkSize, input_size - offset);
        const int dstCapacity = (take == kChunkSize) ? full_chunk_bound : LZ4_compressBound(static_cast<int>(take));
        
        std::string compressed;
        compressed.resize(static_cast<std::size_t>(dstCapacity));
        
        const int n = LZ4_compress_fast(
            src + offset,
            compressed.data(),
            static_cast<int>(take),
            dstCapacity,
            LZ4_ACCELERATION_MAX);
        
        if (n <= 0) {
            return -1;
        }
        compressed.resize(static_cast<std::size_t>(n));
        total += n;
        chunks.push_back(std::move(compressed));
    }
    return total;
}
