#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Fast uint32 to decimal, returns pointer to start of number in buffer
// Buffer must have at least 10 bytes
inline char* u32_to_dec(uint32_t val, char* buf_end) {
    char* p = buf_end;
    do {
        *--p = '0' + (val % 10);
        val /= 10;
    } while (val);
    return p;
}

// Fast uint16 to decimal
inline char* u16_to_dec(uint16_t val, char* buf_end) {
    char* p = buf_end;
    do {
        *--p = '0' + (val % 10);
        val /= 10;
    } while (val);
    return p;
}

// Count digits in uint32
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

inline int count_digits_u16(uint16_t v) {
    if (v < 10) return 1;
    if (v < 100) return 2;
    if (v < 1000) return 3;
    if (v < 10000) return 4;
    return 5;
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
        total_size += count_digits_u32(timestamps[i]) + 1
                    + count_digits_u32(ids[i]) + 1
                    + count_digits_u16(levels[i]) + 1
                    + messages[i].size() + 1;
    }
    
    // Allocate buffer once
    std::string out;
    out.resize(total_size);
    
    char num_buf[12];
    char* num_end = num_buf + 12;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = &out[0];
        
        // FNV-1a constants
        uint64_t h = 1469598103934665603ULL;
        
        for (std::size_t i = 0; i < n; ++i) {
            // Write timestamp
            char* num_start = u32_to_dec(timestamps[i], num_end);
            std::size_t len = num_end - num_start;
            std::memcpy(ptr, num_start, len);
            ptr += len;
            
            *ptr++ = '|';
            
            // Write id
            num_start = u32_to_dec(ids[i], num_end);
            len = num_end - num_start;
            std::memcpy(ptr, num_start, len);
            ptr += len;
            
            *ptr++ = '|';
            
            // Write level
            num_start = u16_to_dec(levels[i], num_end);
            len = num_end - num_start;
            std::memcpy(ptr, num_start, len);
            ptr += len;
            
            *ptr++ = '|';
            
            // Write message
            len = messages[i].size();
            if (len > 0) {
                std::memcpy(ptr, messages[i].data(), len);
                ptr += len;
            }
            
            *ptr++ = '\n';
        }
        
        // Compute hash over the buffer
        const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
        for (std::size_t j = 0; j < total_size; ++j) {
            h ^= static_cast<uint64_t>(data[j]);
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    
    return hash;
}