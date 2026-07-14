#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    chunks.clear();
    if (input.empty()) {
        return 0;
    }

    const std::size_t input_size = input.size();
    const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
    chunks.reserve(num_chunks);

    // Pre-calculate the maximum possible size a single chunk can take to avoid repeated allocations.
    const int max_compressed_size = LZ4_compressBound(static_cast<int>(kChunkSize));
    
    // Use a reusable buffer to avoid zero-initializing a new string's memory every iteration.
    std::vector<char> scratch_buffer(max_compressed_size);

    int total_compressed_size = 0;

    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t current_chunk_size = std::min(kChunkSize, input_size - offset);
        
        const int compressed_size = LZ4_compress_default(
            input.data() + offset,
            scratch_buffer.data(),
            static_cast<int>(current_chunk_size),
            max_compressed_size
        );

        if (compressed_size <= 0) {
            return -1;
        }

        // Construct the string directly from the scratch buffer to minimize copies.
        chunks.emplace_back(scratch_buffer.data(), static_cast<std::size_t>(compressed_size));
        total_compressed_size += compressed_size;
    }

    return total_compressed_size;
}