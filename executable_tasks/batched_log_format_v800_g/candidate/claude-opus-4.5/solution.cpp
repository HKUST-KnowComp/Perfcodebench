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

// Count decimal digits for uint32
inline size_t u32_digits(uint32_t val) {
    if (val == 0) return 1;
    size_t d = 0;
    while (val > 0) { ++d; val /= 10; }
    return d;
}

inline size_t u16_digits(uint16_t val) {
    if (val == 0) return 1;
    size_t d = 0;
    while (val > 0) { ++d; val /= 10; }
    return d;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    const size_t n = ids.size();
    if (n == 0 || iters <= 0) {
        // Empty input - hash of empty string
        return 1469598103934665603ULL;
    }
    
    // Pre-compute total size needed
    size_t total_size = 0;
    for (size_t i = 0; i < n; ++i) {
        // timestamp|id|level|message\n
        total_size += u32_digits(timestamps[i]) + 1  // timestamp + |
                    + u32_digits(ids[i]) + 1         // id + |
                    + u16_digits(levels[i]) + 1      // level + |
                    + messages[i].size() + 1;        // message + \n
    }
    
    // Allocate buffer once
    std::string out;
    out.resize(total_size);
    
    uint64_t hash = 0;
    char num_buf[12];  // Enough for uint32 max (10 digits)
    
    for (int iter = 0; iter < iters; ++iter) {
        char* dst = &out[0];
        
        // FNV-1a hash computed incrementally
        uint64_t h = 1469598103934665603ULL;
        
        for (size_t i = 0; i < n; ++i) {
            // Write timestamp
            char* num_end = num_buf + 11;
            char* num_start = u32_to_str(timestamps[i], num_end);
            size_t len = num_end - num_start;
            std::memcpy(dst, num_start, len);
            for (size_t j = 0; j < len; ++j) {
                h ^= static_cast<uint64_t>(static_cast<unsigned char>(dst[j]));
                h *= 1099511628211ULL;
            }
            dst += len;
            
            // Write |
            *dst++ = '|';
            h ^= static_cast<uint64_t>('|');
            h *= 1099511628211ULL;
            
            // Write id
            num_start = u32_to_str(ids[i], num_end);
            len = num_end - num_start;
            std::memcpy(dst, num_start, len);
            for (size_t j = 0; j < len; ++j) {
                h ^= static_cast<uint64_t>(static_cast<unsigned char>(dst[j]));
                h *= 1099511628211ULL;
            }
            dst += len;
            
            // Write |
            *dst++ = '|';
            h ^= static_cast<uint64_t>('|');
            h *= 1099511628211ULL;
            
            // Write level
            num_start = u16_to_str(levels[i], num_end);
            len = num_end - num_start;
            std::memcpy(dst, num_start, len);
            for (size_t j = 0; j < len; ++j) {
                h ^= static_cast<uint64_t>(static_cast<unsigned char>(dst[j]));
                h *= 1099511628211ULL;
            }
            dst += len;
            
            // Write |
            *dst++ = '|';
            h ^= static_cast<uint64_t>('|');
            h *= 1099511628211ULL;
            
            // Write message
            const std::string& msg = messages[i];
            len = msg.size();
            if (len > 0) {
                std::memcpy(dst, msg.data(), len);
                const unsigned char* mp = reinterpret_cast<const unsigned char*>(dst);
                for (size_t j = 0; j < len; ++j) {
                    h ^= static_cast<uint64_t>(mp[j]);
                    h *= 1099511628211ULL;
                }
                dst += len;
            }
            
            // Write newline
            *dst++ = '\n';
            h ^= static_cast<uint64_t>('\n');
            h *= 1099511628211ULL;
        }
        
        hash = h;
    }
    
    return hash;
}