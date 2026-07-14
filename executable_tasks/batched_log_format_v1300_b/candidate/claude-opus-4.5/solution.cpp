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

// Count decimal digits in uint32
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
    if (n == 0) {
        // Empty input - hash of empty string
        uint64_t hash = 1469598103934665603ULL;
        return hash;
    }
    
    // Pre-compute the exact size needed for the output buffer
    std::size_t total_size = 0;
    for (std::size_t i = 0; i < n; ++i) {
        // timestamp + '|' + id + '|' + level + '|' + message + '\n'
        total_size += count_digits(timestamps[i]) + 1 +
                      count_digits(ids[i]) + 1 +
                      count_digits(levels[i]) + 1 +
                      messages[i].size() + 1;
    }
    
    // Pre-allocate output buffer
    std::string out;
    out.resize(total_size);
    
    char num_buf[12];  // Enough for uint32 max (10 digits)
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        char* dst = &out[0];
        
        // Compute hash incrementally while building output
        hash = 1469598103934665603ULL;
        
        for (std::size_t i = 0; i < n; ++i) {
            // Write timestamp
            char* num_end = num_buf + 11;
            char* num_start = u32_to_dec(timestamps[i], num_end);
            std::size_t num_len = num_end - num_start;
            std::memcpy(dst, num_start, num_len);
            for (std::size_t j = 0; j < num_len; ++j) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(dst[j]));
                hash *= 1099511628211ULL;
            }
            dst += num_len;
            
            // Write '|'
            *dst = '|';
            hash ^= static_cast<uint64_t>('|');
            hash *= 1099511628211ULL;
            ++dst;
            
            // Write id
            num_start = u32_to_dec(ids[i], num_end);
            num_len = num_end - num_start;
            std::memcpy(dst, num_start, num_len);
            for (std::size_t j = 0; j < num_len; ++j) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(dst[j]));
                hash *= 1099511628211ULL;
            }
            dst += num_len;
            
            // Write '|'
            *dst = '|';
            hash ^= static_cast<uint64_t>('|');
            hash *= 1099511628211ULL;
            ++dst;
            
            // Write level
            num_start = u32_to_dec(levels[i], num_end);
            num_len = num_end - num_start;
            std::memcpy(dst, num_start, num_len);
            for (std::size_t j = 0; j < num_len; ++j) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(dst[j]));
                hash *= 1099511628211ULL;
            }
            dst += num_len;
            
            // Write '|'
            *dst = '|';
            hash ^= static_cast<uint64_t>('|');
            hash *= 1099511628211ULL;
            ++dst;
            
            // Write message
            const std::string& msg = messages[i];
            std::size_t msg_len = msg.size();
            if (msg_len > 0) {
                std::memcpy(dst, msg.data(), msg_len);
                for (std::size_t j = 0; j < msg_len; ++j) {
                    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(dst[j]));
                    hash *= 1099511628211ULL;
                }
                dst += msg_len;
            }
            
            // Write '\n'
            *dst = '\n';
            hash ^= static_cast<uint64_t>('\n');
            hash *= 1099511628211ULL;
            ++dst;
        }
    }
    
    return hash;
}