#include "interface.h"
#include "lz4.h"
#include <vector>
#include <string>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
    uint64_t sum = 0;
    std::vector<char> buffer;
    buffer.reserve(64 * 1024);

    for (std::size_t i = 0; i < chunks.size(); ++i) {
        int target_size = sizes[i];
        if (target_size < 0) return 0;

        if (buffer.size() < static_cast<std::size_t>(target_size)) {
            buffer.resize(target_size);
        }

        const std::string& chunk = chunks[i];
        int decoded = LZ4_decompress_safe(
            chunk.data(),
            buffer.data(),
            static_cast<int>(chunk.size()),
            target_size
        );

        if (decoded != target_size) {
            return 0;
        }

        const char* data = buffer.data();
        for (int j = 0; j < target_size; ++j) {
            sum += static_cast<uint64_t>(static_cast<unsigned char>(data[j]));
        }
    }
    return sum;
}