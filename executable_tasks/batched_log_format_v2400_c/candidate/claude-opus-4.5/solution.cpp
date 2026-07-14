#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

// Fast integer to string conversion, writes digits in reverse then reverses
inline char* uint32_to_str(uint32_t val, char* buf) {
    if (val == 0) {
        *buf++ = '0';
        return buf;
    }
    char* start = buf;
    while (val > 0) {
        *buf++ = '0' + (val % 10);
        val /= 10;
    }
    // Reverse the digits
    char* end = buf - 1;
    while (start < end) {
        char tmp = *start;
        *start = *end;
        *end = tmp;
        ++start;
        --end;
    }
    return buf;
}

inline char* uint16_to_str(uint16_t val, char* buf) {
    if (val == 0) {
        *buf++ = '0';
        return buf;
    }
    char* start = buf;
    while (val > 0) {
        *buf++ = '0' + (val % 10);
        val /= 10;
    }
    char* end = buf - 1;
    while (start < end) {
        char tmp = *start;
        *start = *end;
        *end = tmp;
        ++start;
        --end;
    }
    return buf;
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
    
    // Estimate total size: each line has timestamp(max 10) + | + id(max 10) + | + level(max 5) + | + msg + \n
    // Conservative estimate: 30 bytes overhead per line + message lengths
    std::size_t estimated_size = 0;
    for (std::size_t i = 0; i < n; ++i) {
        estimated_size += 32 + messages[i].size();
    }
    
    std::string out;
    out.reserve(estimated_size);
    
    // Temporary buffer for number conversion (max 10 digits for uint32 + null)
    char num_buf[16];
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        
        // Build the output string
        for (std::size_t i = 0; i < n; ++i) {
            // Timestamp
            char* end = uint32_to_str(timestamps[i], num_buf);
            out.append(num_buf, end - num_buf);
            out.push_back('|');
            
            // ID
            end = uint32_to_str(ids[i], num_buf);
            out.append(num_buf, end - num_buf);
            out.push_back('|');
            
            // Level
            end = uint16_to_str(levels[i], num_buf);
            out.append(num_buf, end - num_buf);
            out.push_back('|');
            
            // Message
            out.append(messages[i]);
            out.push_back('\n');
        }
        
        // Compute FNV-1a hash
        hash = 1469598103934665603ULL;
        const char* data = out.data();
        const std::size_t len = out.size();
        for (std::size_t j = 0; j < len; ++j) {
            hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[j]));
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}