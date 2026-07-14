#include "interface.h"
#include "lz4.h"
#include <vector>
#include <string>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
    uint64_t total_sum = 0;
    std::vector<char> buffer;
    
    // Pre-allocate buffer to the maximum required size to avoid repeated reallocations
    int max_size = 0;
    for (int s : sizes) {
        if (s > max_size) max_size = s;
    }
    buffer.resize(max_size);

    for (std::size_t i = 0; i < chunks.size(); ++i) {
        const std::string& compressed = chunks[i];
        const int decompressed_size = sizes[i];
        
        // Decompress directly into the reusable buffer
        const int decoded = LZ4_decompress_safe(
            compressed.data(), 
            buffer.data(), 
            static_cast<int>(compressed.size()), 
            decompressed_size
        );

        if (decoded != decompressed_size) {
            return 0;
        }

        // Fast checksum calculation
        const unsigned char* data = reinterpret_cast<const unsigned char*>(buffer.data());
        uint64_t local_sum = 0;
        for (int j = 0; j < decompressed_size; ++j) {
            local_sum += data[j];
        }
        total_sum += local_sum;
    }

    return total_sum;
}