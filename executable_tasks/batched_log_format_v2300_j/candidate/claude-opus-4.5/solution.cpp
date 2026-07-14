#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Fast integer to decimal string, returns pointer to start of string
// Buffer must have at least 11 bytes for uint32_t
inline char* uint32_to_str(uint32_t val, char* buf_end) {
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

inline char* uint16_to_str(uint16_t val, char* buf_end) {
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

// Count digits in a number
inline int count_digits(uint32_t val) {
    if (val == 0) return 1;
    int count = 0;
    while (val > 0) {
        ++count;
        val /= 10;
    }
    return count;
}

inline int count_digits16(uint16_t val) {
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
        // Empty input - hash of empty string
        return 1469598103934665603ULL;
    }
    
    // Pre-compute all numeric strings once (they don't change between iterations)
    std::vector<std::string> ts_strs(n);
    std::vector<std::string> id_strs(n);
    std::vector<std::string> lv_strs(n);
    
    char buf[12];
    char* buf_end = buf + 11;
    buf[11] = '\0';
    
    std::size_t total_size = 0;
    for (std::size_t i = 0; i < n; ++i) {
        char* p = uint32_to_str(timestamps[i], buf_end);
        ts_strs[i].assign(p, buf_end - p);
        
        p = uint32_to_str(ids[i], buf_end);
        id_strs[i].assign(p, buf_end - p);
        
        p = uint16_to_str(levels[i], buf_end);
        lv_strs[i].assign(p, buf_end - p);
        
        // timestamp + | + id + | + level + | + message + \n
        total_size += ts_strs[i].size() + 1 + id_strs[i].size() + 1 + 
                      lv_strs[i].size() + 1 + messages[i].size() + 1;
    }
    
    // Pre-allocate output buffer
    std::string out;
    out.reserve(total_size);
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Build output and compute hash incrementally
        uint64_t h = 1469598103934665603ULL;
        out.clear();
        
        for (std::size_t i = 0; i < n; ++i) {
            // Append timestamp
            out.append(ts_strs[i]);
            out.push_back('|');
            // Append id
            out.append(id_strs[i]);
            out.push_back('|');
            // Append level
            out.append(lv_strs[i]);
            out.push_back('|');
            // Append message
            out.append(messages[i]);
            out.push_back('\n');
        }
        
        // Compute hash over final buffer
        const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
        const std::size_t len = out.size();
        for (std::size_t j = 0; j < len; ++j) {
            h ^= static_cast<uint64_t>(data[j]);
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    
    return hash;
}