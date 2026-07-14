#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 14;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    chunks.clear();
    if (input.empty()) {
        return 0;
    }

    const std::size_t input_size = input.size();
    const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
    chunks.reserve(num_chunks);

    int total_compressed_size = 0;
    const char* src_ptr = input.data();

    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t current_chunk_size = std::min(kChunkSize, input_size - offset);
        const int max_compressed_size = LZ4_compressBound(static_cast<int>(current_chunk_size));
        
        // Create string and resize once to the maximum possible bound to avoid multiple reallocations
        // However, to be truly efficient with memory and avoid the double-resize pattern,
        // we use a temporary buffer or resize once and then shrink.
        std::string compressed;
        compressed.resize(max_compressed_size);

        const int compressed_size = LZ4_compress_default(
            src_ptr + offset, 
            compressed.data(), 
            static_cast<int>(current_chunk_size), 
            max_compressed_size
        );

        if (compressed_size <= 0) {
            return -1;
        }

        // Shrink to actual size
        compressed.resize(compressed_size);
        total_compressed_size += compressed_size;
        chunks.push_back(std::move(compressed));
    }

    return total_compressed_size;
}