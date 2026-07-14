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
    const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
    chunks.reserve(num_chunks);

    // Pre-calculate max possible size for a chunk to reuse a single buffer
    const int max_compressed_size = LZ4_compressBound(static_cast<int>(kChunkSize));
    std::string buffer;
    buffer.resize(max_compressed_size);

    int total_compressed_size = 0;
    const char* input_ptr = input.data();

    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t take = std::min(kChunkSize, input_size - offset);
        
        // Use LZ4_compress_default for significantly higher throughput than LZ4_compress_HC
        const int compressed_size = LZ4_compress_default(
            input_ptr + offset, 
            &buffer[0], 
            static_cast<int>(take), 
            max_compressed_size
        );

        if (compressed_size <= 0) {
            return -1;
        }

        // Create a string of the exact size needed
        chunks.emplace_back(buffer.data(), compressed_size);
        total_compressed_size += compressed_size;
    }

    return total_compressed_size;
}