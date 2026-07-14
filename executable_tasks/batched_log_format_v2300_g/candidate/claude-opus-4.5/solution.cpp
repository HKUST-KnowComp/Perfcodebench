#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Fast uint32 to string, returns pointer to start of number in buffer
// Buffer must have at least 10 bytes
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

// Fast uint16 to string
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

// Count digits in uint32
inline int count_digits_u32(uint32_t val) {
    if (val == 0) return 1;
    int count = 0;
    while (val > 0) {
        count++;
        val /= 10;
    }
    return count;
}

inline int count_digits_u16(uint16_t val) {
    if (val == 0) return 1;
    int count = 0;
    while (val > 0) {
        count++;
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
    
    const size_t n = ids.size();
    if (n == 0 || iters <= 0) {
        // Empty input - hash of empty string
        return 1469598103934665603ULL;
    }
    
    // Pre-compute string representations and sizes
    std::vector<int> ts_lens(n);
    std::vector<int> id_lens(n);
    std::vector<int> lv_lens(n);
    
    size_t total_size = 0;
    for (size_t i = 0; i < n; ++i) {
        ts_lens[i] = count_digits_u32(timestamps[i]);
        id_lens[i] = count_digits_u32(ids[i]);
        lv_lens[i] = count_digits_u16(levels[i]);
        // timestamp | id | level | message \n
        total_size += ts_lens[i] + 1 + id_lens[i] + 1 + lv_lens[i] + 1 + messages[i].size() + 1;
    }
    
    // Pre-allocate output buffer
    std::string out;
    out.resize(total_size);
    
    char num_buf[12];  // Enough for uint32 max (10 digits)
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = &out[0];
        
        for (size_t i = 0; i < n; ++i) {
            // Write timestamp
            char* num_start = uint32_to_str(timestamps[i], num_buf + 11);
            int len = (num_buf + 11) - num_start;
            std::memcpy(ptr, num_start, len);
            ptr += len;
            
            *ptr++ = '|';
            
            // Write id
            num_start = uint32_to_str(ids[i], num_buf + 11);
            len = (num_buf + 11) - num_start;
            std::memcpy(ptr, num_start, len);
            ptr += len;
            
            *ptr++ = '|';
            
            // Write level
            num_start = uint16_to_str(levels[i], num_buf + 11);
            len = (num_buf + 11) - num_start;
            std::memcpy(ptr, num_start, len);
            ptr += len;
            
            *ptr++ = '|';
            
            // Write message
            size_t msg_len = messages[i].size();
            if (msg_len > 0) {
                std::memcpy(ptr, messages[i].data(), msg_len);
                ptr += msg_len;
            }
            
            *ptr++ = '\n';
        }
        
        // Compute FNV-1a hash
        hash = 1469598103934665603ULL;
        const unsigned char* data = reinterpret_cast<const unsigned char*>(out.data());
        const unsigned char* end = data + total_size;
        while (data < end) {
            hash ^= static_cast<uint64_t>(*data++);
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}