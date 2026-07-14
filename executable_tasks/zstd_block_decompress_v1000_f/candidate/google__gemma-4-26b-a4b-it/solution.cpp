#include "interface.h"
#include "zstd.h"
#include <string>
#include <cstring>

namespace {

// Optimized FNV-1a implementation
// Processes data in 8-byte chunks to reduce loop overhead
inline uint64_t checksum_bytes_optimized(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    const size_t eight_byte_chunks = size / 8;
    const size_t remainder = size % 8;

    const uint64_t* ptr64 = reinterpret_cast<const uint64_t*>(data);
    for (size_t i = 0; i < eight_byte_chunks; ++i) {
        // Note: FNV-1a is byte-oriented. To maintain exact correctness with the 
        // original byte-by-byte loop, we must process bytes. 
        // However, we can unroll the loop to help the compiler vectorize or pipeline.
        // For strict correctness with the original byte-order-dependent FNV-1a:
        const uint8_t* byte_ptr = reinterpret_cast<const uint8_t*>(&ptr64[i]);
        for(int j = 0; j < 8; ++j) {
            hash ^= static_cast<uint64_t>(byte_ptr[j]);
            hash *= prime;
        }
    }

    const uint8_t* final_ptr = reinterpret_cast<const uint8_t*>(data + (eight_byte_chunks * 8));
    for (size_t i = 0; i < remainder; ++i) {
        hash ^= static_cast<uint64_t>(final_ptr[i]);
        hash *= prime;
    }

    return hash;
}

// Re-implementing the exact byte-by-byte logic but unrolled for performance
inline uint64_t checksum_bytes_fast(const char* data, std::size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);

    size_t i = 0;
    // Unroll by 4 to improve ILP
    for (; i + 3 < size; i += 4) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[i+1]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[i+2]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(ptr[i+3]);
        hash *= prime;
    }
    for (; i < size; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= prime;
    }
    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    // Use a vector or a unique_ptr to avoid string overhead if necessary,
    // but std::string is fine as long as we don't copy the input.
    std::string restored;
    restored.resize(decompressed_size);

    // Decompress directly from the input string buffer to the output buffer
    size_t const dSize = ZSTD_decompress(
        restored.data(), restored.size(), 
        compressed.data(), compressed.size());

    if (ZSTD_isError(dSize) || dSize != decompressed_size) {
        return 0;
    }

    return checksum_bytes_fast(restored.data(), decompressed_size);
}
