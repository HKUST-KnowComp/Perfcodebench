#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>
#include <string>

namespace {
// Increasing chunk size to 1MB to reduce the number of chunks and function call overhead
// while remaining within reasonable memory bounds for large payloads.
constexpr std::size_t kChunkSize = 1024 * 1024;
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
        
        // Pre-calculate bound to avoid multiple calls
        const int max_compressed_size = LZ4_compressBound(static_cast<int>(current_chunk_size));
        
        // Use a temporary buffer to perform compression to avoid multiple string reallocations
        // and to allow us to use the most efficient string construction method.
        std::string compressed_chunk;
        compressed_chunk.resize(max_compressed_size);

        const int compressed_size = LZ4_compress_default(
            src_ptr + offset,
            compressed_chunk.data(),
            static_cast<int>(current_chunk_size),
            max_compressed_size
        );

        if (compressed_size <= 0) {
            return -1;
        }

        // Shrink the string to the actual compressed size
        compressed_chunk.resize(compressed_size);
        total_compressed_size += compressed_size;
        
        chunks.push_back(std::move(compressed_chunk));
    }

    return total_compressed_size;
}