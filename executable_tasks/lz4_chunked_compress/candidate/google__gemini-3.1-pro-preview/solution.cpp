#include "interface.h"
#include "lz4.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 16;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    const std::size_t input_size = input.size();
    if (input_size == 0) {
        chunks.clear();
        return 0;
    }

    const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
    chunks.clear();
    chunks.resize(num_chunks);

    int total_compressed_size = 0;
    const char* input_ptr = input.data();

    for (std::size_t i = 0; i < num_chunks; ++i) {
        const std::size_t offset = i * kChunkSize;
        const std::size_t remaining = input_size - offset;
        const int src_size = static_cast<int>(std::min(kChunkSize, remaining));
        
        // Calculate maximum possible compressed size for this chunk
        const int max_dst_size = LZ4_compressBound(src_size);
        
        // Prepare the destination string
        std::string& compressed = chunks[i];
        compressed.resize(static_cast<std::size_t>(max_dst_size));

        // Use LZ4_compress_default for significantly higher throughput than LZ4_compress_HC
        const int compressed_size = LZ4_compress_default(
            input_ptr + offset, 
            &compressed[0], 
            src_size, 
            max_dst_size
        );

        if (compressed_size <= 0) {
            chunks.clear();
            return -1;
        }

        // Shrink the string to the actual compressed size
        compressed.resize(static_cast<std::size_t>(compressed_size));
        total_compressed_size += compressed_size;
    }

    return total_compressed_size;
}