#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>
#include <string>

namespace {
// Increased chunk size to 1MB to reduce overhead and improve LZ4 dictionary efficiency
constexpr std::size_t kChunkSize = 1024 * 1024;
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    chunks.clear();
    if (input.empty()) return 0;

    const std::size_t input_size = input.size();
    const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
    chunks.reserve(num_chunks);

    // Pre-allocate a reusable buffer for compression to avoid repeated allocations
    // LZ4_compressBound for 1MB is slightly larger than 1MB
    std::vector<char> compression_buffer(LZ4_compressBound(static_cast<int>(kChunkSize)));
    
    int total_compressed_size = 0;
    const char* input_ptr = input.data();

    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t take = std::min(kChunkSize, input_size - offset);
        
        // Use LZ4_compress_fast for better throughput; it's still lossless
        const int compressed_size = LZ4_compress_fast(
            input_ptr + offset, 
            compression_buffer.data(), 
            static_cast<int>(take), 
            static_cast<int>(compression_buffer.size()),
            1 // acceleration level
        );

        if (compressed_size <= 0) {
            return -1;
        }

        // Construct the string directly from the buffer
        chunks.emplace_back(compression_buffer.data(), static_cast<std::size_t>(compressed_size));
        total_compressed_size += compressed_size;
    }

    return total_compressed_size;
}