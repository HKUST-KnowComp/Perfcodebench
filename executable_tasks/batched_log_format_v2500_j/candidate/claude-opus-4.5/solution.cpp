#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Count digits in a 32-bit unsigned integer
inline int count_digits_u32(uint32_t v) {
    if (v < 10) return 1;
    if (v < 100) return 2;
    if (v < 1000) return 3;
    if (v < 10000) return 4;
    if (v < 100000) return 5;
    if (v < 1000000) return 6;
    if (v < 10000000) return 7;
    if (v < 100000000) return 8;
    if (v < 1000000000) return 9;
    return 10;
}

// Count digits in a 16-bit unsigned integer
inline int count_digits_u16(uint16_t v) {
    if (v < 10) return 1;
    if (v < 100) return 2;
    if (v < 1000) return 3;
    if (v < 10000) return 4;
    return 5;
}

// Write uint32_t to buffer, return pointer past end
inline char* write_u32(char* buf, uint32_t v) {
    int len = count_digits_u32(v);
    char* end = buf + len;
    char* p = end;
    do {
        *--p = '0' + (v % 10);
        v /= 10;
    } while (v > 0);
    return end;
}

// Write uint16_t to buffer, return pointer past end
inline char* write_u16(char* buf, uint16_t v) {
    int len = count_digits_u16(v);
    char* end = buf + len;
    char* p = end;
    do {
        *--p = '0' + (v % 10);
        v /= 10;
    } while (v > 0);
    return end;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    const size_t n = ids.size();
    if (n == 0) {
        uint64_t hash = 1469598103934665603ULL;
        return hash;
    }
    
    // Pre-compute the total size needed
    size_t total_size = 0;
    for (size_t i = 0; i < n; ++i) {
        // timestamp + '|' + id + '|' + level + '|' + message + '\n'
        total_size += count_digits_u32(timestamps[i]) + 1 +
                      count_digits_u32(ids[i]) + 1 +
                      count_digits_u16(levels[i]) + 1 +
                      messages[i].size() + 1;
    }
    
    // Allocate buffer once
    std::string out;
    out.resize(total_size);
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = &out[0];
        
        // Build the output and compute hash incrementally
        hash = 1469598103934665603ULL;
        
        for (size_t i = 0; i < n; ++i) {
            char* line_start = ptr;
            
            // Write timestamp
            ptr = write_u32(ptr, timestamps[i]);
            *ptr++ = '|';
            
            // Write id
            ptr = write_u32(ptr, ids[i]);
            *ptr++ = '|';
            
            // Write level
            ptr = write_u16(ptr, levels[i]);
            *ptr++ = '|';
            
            // Write message
            const std::string& msg = messages[i];
            std::memcpy(ptr, msg.data(), msg.size());
            ptr += msg.size();
            
            // Write newline
            *ptr++ = '\n';
            
            // Compute hash for this line
            for (const char* p = line_start; p < ptr; ++p) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*p));
                hash *= 1099511628211ULL;
            }
        }
    }
    
    return hash;
}