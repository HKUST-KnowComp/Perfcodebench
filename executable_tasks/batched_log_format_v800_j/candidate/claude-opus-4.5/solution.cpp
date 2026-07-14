#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Fast integer to decimal string, returns pointer to start of string
// Buffer must have at least 10 bytes for uint32_t
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

// Count decimal digits
inline int count_digits(uint32_t val) {
    if (val == 0) return 1;
    int count = 0;
    while (val > 0) {
        ++count;
        val /= 10;
    }
    return count;
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
        // Empty input: hash of empty string
        return 1469598103934665603ULL;
    }
    
    // Pre-compute total size needed
    std::size_t total_size = 0;
    for (std::size_t i = 0; i < n; ++i) {
        // timestamp + '|' + id + '|' + level + '|' + message + '\n'
        total_size += count_digits(timestamps[i]) + 1 +
                      count_digits(ids[i]) + 1 +
                      count_digits(levels[i]) + 1 +
                      messages[i].size() + 1;
    }
    
    // Allocate buffer once
    std::string out;
    out.resize(total_size);
    char* const out_begin = &out[0];
    
    // Temp buffer for integer conversion (max 10 digits for uint32_t)
    char int_buf[12];
    char* const int_buf_end = int_buf + 12;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        char* dst = out_begin;
        
        // Build output and compute hash incrementally
        uint64_t h = 1469598103934665603ULL;
        
        for (std::size_t i = 0; i < n; ++i) {
            // Write timestamp
            char* p = u32_to_str(timestamps[i], int_buf_end);
            while (p < int_buf_end) {
                unsigned char c = static_cast<unsigned char>(*p++);
                *dst++ = c;
                h ^= c;
                h *= 1099511628211ULL;
            }
            
            // Write '|'
            *dst++ = '|';
            h ^= '|';
            h *= 1099511628211ULL;
            
            // Write id
            p = u32_to_str(ids[i], int_buf_end);
            while (p < int_buf_end) {
                unsigned char c = static_cast<unsigned char>(*p++);
                *dst++ = c;
                h ^= c;
                h *= 1099511628211ULL;
            }
            
            // Write '|'
            *dst++ = '|';
            h ^= '|';
            h *= 1099511628211ULL;
            
            // Write level
            p = u32_to_str(levels[i], int_buf_end);
            while (p < int_buf_end) {
                unsigned char c = static_cast<unsigned char>(*p++);
                *dst++ = c;
                h ^= c;
                h *= 1099511628211ULL;
            }
            
            // Write '|'
            *dst++ = '|';
            h ^= '|';
            h *= 1099511628211ULL;
            
            // Write message
            const std::string& msg = messages[i];
            const char* msg_data = msg.data();
            const std::size_t msg_len = msg.size();
            std::memcpy(dst, msg_data, msg_len);
            for (std::size_t j = 0; j < msg_len; ++j) {
                unsigned char c = static_cast<unsigned char>(msg_data[j]);
                h ^= c;
                h *= 1099511628211ULL;
            }
            dst += msg_len;
            
            // Write '\n'
            *dst++ = '\n';
            h ^= '\n';
            h *= 1099511628211ULL;
        }
        
        hash = h;
    }
    
    return hash;
}