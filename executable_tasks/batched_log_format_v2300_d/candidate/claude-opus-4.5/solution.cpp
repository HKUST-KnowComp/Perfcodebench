#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Fast integer to string conversion, returns pointer to start of number in buffer
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

// Estimate max digits needed: uint32_t max is 10 digits, uint16_t max is 5 digits
constexpr size_t MAX_U32_DIGITS = 10;
constexpr size_t MAX_U16_DIGITS = 5;

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
    
    // Calculate total message length
    size_t total_msg_len = 0;
    for (size_t i = 0; i < n; ++i) {
        total_msg_len += messages[i].size();
    }
    
    // Max size per line: timestamp(10) + | + id(10) + | + level(5) + | + msg + \n
    // = 10 + 1 + 10 + 1 + 5 + 1 + msg_len + 1 = 29 + msg_len
    const size_t max_size = n * 29 + total_msg_len;
    
    std::string out;
    out.reserve(max_size);
    
    char num_buf[MAX_U32_DIGITS];
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        
        // Build output and compute hash incrementally
        hash = 1469598103934665603ULL;
        
        for (size_t i = 0; i < n; ++i) {
            // Timestamp
            char* start = u32_to_str(timestamps[i], num_buf + MAX_U32_DIGITS);
            size_t len = (num_buf + MAX_U32_DIGITS) - start;
            for (size_t j = 0; j < len; ++j) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(start[j]));
                hash *= 1099511628211ULL;
            }
            out.append(start, len);
            
            // Separator |
            hash ^= static_cast<uint64_t>('|');
            hash *= 1099511628211ULL;
            out += '|';
            
            // ID
            start = u32_to_str(ids[i], num_buf + MAX_U32_DIGITS);
            len = (num_buf + MAX_U32_DIGITS) - start;
            for (size_t j = 0; j < len; ++j) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(start[j]));
                hash *= 1099511628211ULL;
            }
            out.append(start, len);
            
            // Separator |
            hash ^= static_cast<uint64_t>('|');
            hash *= 1099511628211ULL;
            out += '|';
            
            // Level
            start = u16_to_str(levels[i], num_buf + MAX_U16_DIGITS);
            len = (num_buf + MAX_U16_DIGITS) - start;
            for (size_t j = 0; j < len; ++j) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(start[j]));
                hash *= 1099511628211ULL;
            }
            out.append(start, len);
            
            // Separator |
            hash ^= static_cast<uint64_t>('|');
            hash *= 1099511628211ULL;
            out += '|';
            
            // Message
            const std::string& msg = messages[i];
            const char* msg_data = msg.data();
            size_t msg_len = msg.size();
            for (size_t j = 0; j < msg_len; ++j) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(msg_data[j]));
                hash *= 1099511628211ULL;
            }
            out.append(msg_data, msg_len);
            
            // Newline
            hash ^= static_cast<uint64_t>('\n');
            hash *= 1099511628211ULL;
            out += '\n';
        }
    }
    
    return hash;
}