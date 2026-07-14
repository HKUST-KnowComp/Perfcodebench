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
    const std::size_t input_size = input.size();
    if (input_size == 0) return 0;

    const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
    chunks.reserve(num_chunks);

    // Pre-allocate a buffer for the maximum possible compressed size of one chunk
    // to avoid repeated allocations inside the loop.
    std::vector<char> compress_buf(LZ4_compressBound(static_cast<int>(kChunkSize)));
    
    int total_compressed_size = 0;
    const char* input_ptr = input.data();

    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t take = std::min(kChunkSize, input_size - offset);
        
        const int compressed_size = LZ4_compress_default(
            input_ptr + offset, 
            compress_buf.data(), 
            static_cast<int>(take), 
            static_cast<int>(compress_buf.size())
        );

        if (compressed_size <= 0) {
            return -1;
        }

        // Construct the string directly from the buffer slice
        chunks.emplace_back(compress_buf.data(), static_cast<std::size_t>(compressed_size));
        total_compressed_size += compressed_size;
    }

    return total_compressed_size;
}