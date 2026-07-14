#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Count digits in a 32-bit unsigned integer
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

// Fast uint32 to string, writes digits and returns pointer past last digit
inline char* uint32_to_str(uint32_t n, char* buf) {
    int len = count_digits(n);
    char* end = buf + len;
    char* p = end;
    do {
        *--p = '0' + (n % 10);
        n /= 10;
    } while (n > 0);
    return end;
}

// Fast uint16 to string
inline char* uint16_to_str(uint16_t n, char* buf) {
    int len;
    if (n < 10) len = 1;
    else if (n < 100) len = 2;
    else if (n < 1000) len = 3;
    else if (n < 10000) len = 4;
    else len = 5;
    
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
    
    const size_t n = ids.size();
    if (n == 0) {
        // Empty input - hash of empty string
        uint64_t hash = 1469598103934665603ULL;
        return hash;
    }
    
    // Pre-calculate total size needed
    size_t total_size = 0;
    for (size_t i = 0; i < n; ++i) {
        // timestamp + '|' + id + '|' + level + '|' + message + '\n'
        total_size += count_digits(timestamps[i]) + 1 +
                      count_digits(ids[i]) + 1 +
                      (levels[i] < 10 ? 1 : (levels[i] < 100 ? 2 : (levels[i] < 1000 ? 3 : (levels[i] < 10000 ? 4 : 5)))) + 1 +
                      messages[i].size() + 1;
    }
    
    // Allocate buffer once
    std::string out;
    out.resize(total_size);
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Build output and compute hash in one pass
        hash = 1469598103934665603ULL;
        char* ptr = &out[0];
        
        for (size_t i = 0; i < n; ++i) {
            // Write timestamp
            ptr = uint32_to_str(timestamps[i], ptr);
            *ptr++ = '|';
            
            // Write id
            ptr = uint32_to_str(ids[i], ptr);
            *ptr++ = '|';
            
            // Write level
            ptr = uint16_to_str(levels[i], ptr);
            *ptr++ = '|';
            
            // Write message
            const std::string& msg = messages[i];
            std::memcpy(ptr, msg.data(), msg.size());
            ptr += msg.size();
            
            // Write newline
            *ptr++ = '\n';
        }
        
        // Compute FNV-1a hash
        const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
        for (size_t i = 0; i < total_size; ++i) {
            hash ^= static_cast<uint64_t>(data[i]);
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}