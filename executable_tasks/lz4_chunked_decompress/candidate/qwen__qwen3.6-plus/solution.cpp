#include "interface.h"
#include "lz4.h"
#include <vector>
#include <cstdint>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
    uint64_t sum = 0;
    std::vector<char> buffer;
    
    for (size_t i = 0; i < chunks.size(); ++i) {
        int out_size = sizes[i];
        if (out_size <= 0) continue;
        
        if (static_cast<int>(buffer.capacity()) < out_size) {
            buffer.reserve(out_size);
        }
        
        int decoded = LZ4_decompress_safe(
            chunks[i].data(), buffer.data(), static_cast<int>(chunks[i].size()), out_size);
            
        if (decoded != out_size) {
            return 0;
        }
        
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(buffer.data());
        for (int j = 0; j < out_size; ++j) {
            sum += ptr[j];
        }
    }
    return sum;
}