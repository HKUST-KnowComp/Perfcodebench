#include "interface.h"
#include "lz4.h"
#include <vector>
#include <string>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
    uint64_t sum = 0;
    // Reuse a single buffer to avoid repeated allocations/deallocations
    std::vector<char> buffer;
    
    for (std::size_t i = 0; i < chunks.size(); ++i) {
        const std::string& compressed = chunks[i];
        int target_size = sizes[i];
        
        if (buffer.size() < static_cast<size_t>(target_size)) {
            buffer.resize(target_size);
        }

        // Decompress directly from the input string reference to the reusable buffer
        int decoded = LZ4_decompress_safe(
            compressed.data(), 
            buffer.data(), 
            static_cast<int>(compressed.size()), 
            target_size
        );

        if (decoded != target_size) {
            return 0;
        }

        // Fast checksum calculation using raw pointers
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(buffer.data());
        const unsigned char* end = ptr + target_size;
        while (ptr < end) {
            sum += *ptr++;
        }
    }
    return sum;
}