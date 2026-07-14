#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <vector>
#include <string>
#include <memory>

namespace {
// 16KB is a standard medium chunk size that fits well in L1/L2 caches.
constexpr std::size_t kChunkSize = 1 << 14;

// Use a custom deleter for the LZ4 state to manage it with unique_ptr
struct LZ4Deleter {
    void operator()(void* state) const {
        if (state) free(state);
    }
};
}

int compress_chunks(const std::string& input, std::vector<std::string>& chunks) {
    const std::size_t input_size = input.size();
    chunks.clear();
    
    if (input_size == 0) {
        return 0;
    }

    const std::size_t num_chunks = (input_size + kChunkSize - 1) / kChunkSize;
    chunks.reserve(num_chunks);

    // Allocate LZ4 state once to reuse across all chunks.
    // LZ4_compress_fast_extState is faster than LZ4_compress_default because it avoids 
    // the overhead of zeroing the state on every call.
    std::unique_ptr<void, LZ4Deleter> state(malloc(LZ4_sizeofState()));
    if (!state) return -1;

    int total_compressed_size = 0;
    const char* input_ptr = input.data();
    const int acceleration = 1; // Default acceleration for LZ4_compress_fast

    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t take = std::min(kChunkSize, input_size - offset);
        const int max_dst_size = LZ4_compressBound(static_cast<int>(take));

        chunks.emplace_back();
        std::string& compressed = chunks.back();
        
        // Pre-allocate the maximum possible size for the compressed chunk.
        compressed.resize(max_dst_size);

        // Perform compression using the pre-allocated state.
        const int compressed_bytes = LZ4_compress_fast_extState(
            state.get(),
            input_ptr + offset,
            &compressed[0],
            static_cast<int>(take),
            max_dst_size,
            acceleration
        );

        if (compressed_bytes <= 0) {
            return -1;
        }

        // Shrink the string to the actual compressed size.
        compressed.resize(static_cast<std::size_t>(compressed_bytes));
        total_compressed_size += compressed_bytes;
    }

    return total_compressed_size;
}
