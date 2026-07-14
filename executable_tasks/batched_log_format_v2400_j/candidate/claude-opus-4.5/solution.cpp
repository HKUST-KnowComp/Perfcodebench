#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Count digits in a uint32_t
inline int count_digits(uint32_t n) {
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    return 10;
}

// Count digits in a uint16_t
inline int count_digits16(uint16_t n) {
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    return 5;
}

// Write uint32_t to buffer, return pointer past end
inline char* write_uint32(char* buf, uint32_t n) {
    int len = count_digits(n);
    char* end = buf + len;
    char* p = end;
    do {
        *--p = '0' + (n % 10);
        n /= 10;
    } while (n > 0);
    return end;
}

// Write uint16_t to buffer, return pointer past end
inline char* write_uint16(char* buf, uint16_t n) {
    int len = count_digits16(n);
    char* end = buf + len;
    char* p = end;
    do {
        *--p = '0' + (n % 10);
        n /= 10;
    } while (n > 0);
    return end;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    const std::size_t n = ids.size();
    if (n == 0 || iters <= 0) {
        // Empty input - hash of empty string
        return 1469598103934665603ULL;
    }
    
    // Pre-compute total size needed
    std::size_t total_size = 0;
    for (std::size_t i = 0; i < n; ++i) {
        // timestamp + '|' + id + '|' + level + '|' + message + '\n'
        total_size += count_digits(timestamps[i]) + 1 +
                      count_digits(ids[i]) + 1 +
                      count_digits16(levels[i]) + 1 +
                      messages[i].size() + 1;
    }
    
    // Allocate buffer once
    std::string out;
    out.resize(total_size);
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = &out[0];
        
        // Build output and compute hash in single pass
        uint64_t h = 1469598103934665603ULL;
        
        for (std::size_t i = 0; i < n; ++i) {
            char* line_start = ptr;
            
            // Write timestamp
            ptr = write_uint32(ptr, timestamps[i]);
            *ptr++ = '|';
            
            // Write id
            ptr = write_uint32(ptr, ids[i]);
            *ptr++ = '|';
            
            // Write level
            ptr = write_uint16(ptr, levels[i]);
            *ptr++ = '|';
            
            // Write message
            const std::string& msg = messages[i];
            std::memcpy(ptr, msg.data(), msg.size());
            ptr += msg.size();
            
            // Write newline
            *ptr++ = '\n';
            
            // Hash this line
            for (const char* p = line_start; p < ptr; ++p) {
                h ^= static_cast<uint64_t>(static_cast<unsigned char>(*p));
                h *= 1099511628211ULL;
            }
        }
        
        hash = h;
    }
    
    return hash;
}