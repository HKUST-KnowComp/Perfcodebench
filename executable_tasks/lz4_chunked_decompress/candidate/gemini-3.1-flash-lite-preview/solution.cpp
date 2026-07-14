#include "interface.h"
#include "lz4.h"
#include <vector>
#include <algorithm>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
    uint64_t sum = 0;
    int max_size = 0;
    for (int s : sizes) {
        if (s > max_size) max_size = s;
    }

    std::vector<char> buffer(max_size);
    char* buf_ptr = buffer.data();

    for (std::size_t i = 0; i < chunks.size(); ++i) {
        const std::string& chunk = chunks[i];
        int expected_size = sizes[i];
        
        int decoded = LZ4_decompress_safe(chunk.data(), buf_ptr, static_cast<int>(chunk.size()), expected_size);
        
        if (decoded != expected_size) {
            return 0;
        }

        const unsigned char* p = reinterpret_cast<const unsigned char*>(buf_ptr);
        const unsigned char* end = p + expected_size;
        while (p < end) {
            sum += *p++;
        }
    }
    return sum;
}