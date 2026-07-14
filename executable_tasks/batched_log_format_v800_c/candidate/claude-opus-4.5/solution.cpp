#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Fast uint32 to decimal string, returns pointer to start of number
// Buffer must have at least 10 bytes
inline char* u32_to_str(uint32_t val, char* buf_end) {
    char* p = buf_end;
    if (val == 0) {
        *--p = '0';
        return p;
    }
    while (val > 0) {
        *--p = '0' + (val % 10);
        val /= 10;
    }
    return p;
}

// Fast uint16 to decimal string
inline char* u16_to_str(uint16_t val, char* buf_end) {
    char* p = buf_end;
    if (val == 0) {
        *--p = '0';
        return p;
    }
    while (val > 0) {
        *--p = '0' + (val % 10);
        val /= 10;
    }
    return p;
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
    
    // Estimate total size needed
    // Each line: timestamp(max 10) + | + id(max 10) + | + level(max 5) + | + msg + \n
    // = 28 + msg.size() per line
    std::size_t total_size = 0;
    for (std::size_t i = 0; i < n; ++i) {
        total_size += 28 + messages[i].size();
    }
    
    std::string out;
    out.reserve(total_size);
    
    char num_buf[12];  // Buffer for number conversion
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        
        // Build output and compute hash incrementally
        uint64_t h = 1469598103934665603ULL;
        
        for (std::size_t i = 0; i < n; ++i) {
            // Timestamp
            char* p = u32_to_str(timestamps[i], num_buf + 11);
            while (p < num_buf + 11) {
                unsigned char c = static_cast<unsigned char>(*p++);
                h ^= static_cast<uint64_t>(c);
                h *= 1099511628211ULL;
            }
            
            // Separator |
            h ^= static_cast<uint64_t>('|');
            h *= 1099511628211ULL;
            
            // ID
            p = u32_to_str(ids[i], num_buf + 11);
            while (p < num_buf + 11) {
                unsigned char c = static_cast<unsigned char>(*p++);
                h ^= static_cast<uint64_t>(c);
                h *= 1099511628211ULL;
            }
            
            // Separator |
            h ^= static_cast<uint64_t>('|');
            h *= 1099511628211ULL;
            
            // Level
            p = u16_to_str(levels[i], num_buf + 11);
            while (p < num_buf + 11) {
                unsigned char c = static_cast<unsigned char>(*p++);
                h ^= static_cast<uint64_t>(c);
                h *= 1099511628211ULL;
            }
            
            // Separator |
            h ^= static_cast<uint64_t>('|');
            h *= 1099511628211ULL;
            
            // Message
            const std::string& msg = messages[i];
            for (std::size_t j = 0; j < msg.size(); ++j) {
                unsigned char c = static_cast<unsigned char>(msg[j]);
                h ^= static_cast<uint64_t>(c);
                h *= 1099511628211ULL;
            }
            
            // Newline
            h ^= static_cast<uint64_t>('\n');
            h *= 1099511628211ULL;
        }
        
        hash = h;
    }
    
    return hash;
}