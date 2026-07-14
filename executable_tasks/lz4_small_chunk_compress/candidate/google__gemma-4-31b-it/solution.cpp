#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 12;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    chunks.clear();
    if (input.empty()) return 0;

    const std::size_t input_size = input.size();
    const char* input_ptr = input.data();
    
    // Pre-calculate max possible size for a single chunk to reuse a buffer
    const int max_compressed_size = LZ4_compressBound(static_cast<int>(kChunkSize));
    std::vector<char> compression_buffer(max_compressed_size);

    int total_compressed_size = 0;
    
    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t take = std::min(kChunkSize, input_size - offset);
        
        // Compress directly from the input string buffer to avoid substr() allocation
        const int compressed_size = LZ4_compress_default(
            input_ptr + offset, 
            compression_buffer.data(), 
            static_cast<int>(take), 
            max_compressed_size
        );

        if (compressed_size <= 0) {
            return -1;
        }

        // Construct string directly from the buffer
        chunks.emplace_back(compression_buffer.data(), static_cast<std::size_t>(compressed_size));
        total_compressed_size += compressed_size;
    }

    return total_compressed_size;
}