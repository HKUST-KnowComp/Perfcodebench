#include "interface.h"
#include "lz4.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
    uint64_t sum = 0;
    std::vector<char> buffer;  // reused across chunks to avoid repeated allocation
    buffer.reserve(64 * 1024); // small initial capacity hint

    for (std::size_t i = 0; i < chunks.size(); ++i) {
        const std::string& chunk = chunks[i];
        int expected = sizes[i];
        if (expected <= 0) continue;

        // Ensure buffer is large enough (only reallocate when needed)
        if (static_cast<int>(buffer.size()) < expected) {
            buffer.resize(expected);
        }

        int decoded = LZ4_decompress_safe(chunk.data(), buffer.data(),
                                          static_cast<int>(chunk.size()), expected);
        if (decoded != expected) {
            return 0;
        }

        // Sum the decompressed bytes
        const char* data = buffer.data();
        for (int j = 0; j < decoded; ++j) {
            sum += static_cast<uint64_t>(static_cast<unsigned char>(data[j]));
        }
    }
    return sum;
}
