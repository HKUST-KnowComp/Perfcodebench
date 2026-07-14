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
    
    // Pre-compute sizes and cache formatted numbers
    std::vector<char> ts_buf(n * 11);  // max 10 digits + null per timestamp
    std::vector<char> id_buf(n * 11);
    std::vector<char> lv_buf(n * 6);   // max 5 digits for uint16
    std::vector<const char*> ts_ptrs(n);
    std::vector<const char*> id_ptrs(n);
    std::vector<const char*> lv_ptrs(n);
    std::vector<int> ts_lens(n);
    std::vector<int> id_lens(n);
    std::vector<int> lv_lens(n);
    
    size_t total_size = 0;
    
    for (size_t i = 0; i < n; ++i) {
        // Format timestamp
        char* ts_end = ts_buf.data() + (i + 1) * 11;
        char* ts_start = u32_to_str(timestamps[i], ts_end);
        ts_ptrs[i] = ts_start;
        ts_lens[i] = static_cast<int>(ts_end - ts_start);
        
        // Format id
        char* id_end = id_buf.data() + (i + 1) * 11;
        char* id_start = u32_to_str(ids[i], id_end);
        id_ptrs[i] = id_start;
        id_lens[i] = static_cast<int>(id_end - id_start);
        
        // Format level
        char* lv_end = lv_buf.data() + (i + 1) * 6;
        char* lv_start = u16_to_str(levels[i], lv_end);
        lv_ptrs[i] = lv_start;
        lv_lens[i] = static_cast<int>(lv_end - lv_start);
        
        // timestamp|id|level|message\n
        total_size += ts_lens[i] + 1 + id_lens[i] + 1 + lv_lens[i] + 1 + messages[i].size() + 1;
    }
    
    // Pre-allocate output buffer
    std::string out;
    out.resize(total_size);
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Build output and compute hash incrementally
        char* ptr = &out[0];
        uint64_t h = 1469598103934665603ULL;
        
        for (size_t i = 0; i < n; ++i) {
            // Copy timestamp
            std::memcpy(ptr, ts_ptrs[i], ts_lens[i]);
            for (int j = 0; j < ts_lens[i]; ++j) {
                h ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[j]));
                h *= 1099511628211ULL;
            }
            ptr += ts_lens[i];
            
            // Separator
            *ptr++ = '|';
            h ^= static_cast<uint64_t>('|');
            h *= 1099511628211ULL;
            
            // Copy id
            std::memcpy(ptr, id_ptrs[i], id_lens[i]);
            for (int j = 0; j < id_lens[i]; ++j) {
                h ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[j]));
                h *= 1099511628211ULL;
            }
            ptr += id_lens[i];
            
            // Separator
            *ptr++ = '|';
            h ^= static_cast<uint64_t>('|');
            h *= 1099511628211ULL;
            
            // Copy level
            std::memcpy(ptr, lv_ptrs[i], lv_lens[i]);
            for (int j = 0; j < lv_lens[i]; ++j) {
                h ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[j]));
                h *= 1099511628211ULL;
            }
            ptr += lv_lens[i];
            
            // Separator
            *ptr++ = '|';
            h ^= static_cast<uint64_t>('|');
            h *= 1099511628211ULL;
            
            // Copy message
            const std::string& msg = messages[i];
            const size_t msg_len = msg.size();
            std::memcpy(ptr, msg.data(), msg_len);
            for (size_t j = 0; j < msg_len; ++j) {
                h ^= static_cast<uint64_t>(static_cast<unsigned char>(ptr[j]));
                h *= 1099511628211ULL;
            }
            ptr += msg_len;
            
            // Newline
            *ptr++ = '\n';
            h ^= static_cast<uint64_t>('\n');
            h *= 1099511628211ULL;
        }
        
        hash = h;
    }
    
    return hash;
}