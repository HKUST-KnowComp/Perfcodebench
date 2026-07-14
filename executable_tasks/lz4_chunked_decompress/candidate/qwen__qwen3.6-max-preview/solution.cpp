#include "interface.h"
#include "lz4.h"
#include <algorithm>
#include <cstddef>
#include <vector>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
    uint64_t sum = 0;
    if (chunks.empty()) return sum;

    size_t max_size = 0;
    for (int s : sizes) {
        if (static_cast<size_t>(s) > max_size) max_size = static_cast<size_t>(s);
    }
    
    // Pre-allocate buffer once to avoid per-chunk allocation overhead
    std::vector<char> buffer(std::max(max_size, size_t(1)));

    for (size_t i = 0; i < chunks.size(); ++i) {
        const std::string& chunk = chunks[i];
        int target_size = sizes[i];
        
        int decoded = LZ4_decompress_safe(
            chunk.data(), 
            buffer.data(), 
            static_cast<int>(chunk.size()), 
            target_size
        );
        
        if (decoded != target_size) {
            return 0;
        }

        uint64_t local_sum = 0;
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(buffer.data());
        const unsigned char* end = ptr + target_size;
        while (ptr < end) {
            local_sum += *ptr++;
        }
        sum += local_sum;
    }
    return sum;
}
