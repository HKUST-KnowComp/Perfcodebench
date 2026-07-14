#include "interface.h"

#include <string>
#include <vector>
#include <cstring>

namespace {

// Fast integer to string conversion that writes directly to a buffer
inline char* write_uint(char* buf, uint32_t val) {
    if (val == 0) {
        *buf++ = '0';
        return buf;
    }
    char temp[10];
    int len = 0;
    while (val > 0) {
        temp[len++] = (val % 10) + '0';
        val /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        *buf++ = temp[i];
    }
    return buf;
}

// FNV-1a hash update
inline uint64_t update_hash(uint64_t hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= 1099511628211ULL;
    return hash;
}

inline uint64_t update_hash_str(uint64_t hash, const std::string& s) {
    for (char c : s) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    uint64_t final_hash = 0;
    char num_buf[12];

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        
        for (std::size_t i = 0; i < ids.size(); ++i) {
            // Timestamp
            char* p = write_uint(num_buf, timestamps[i]);
            for (char* b = num_buf; b != p; ++b) hash = update_hash(hash, *b);
            
            // Separator
            hash = update_hash(hash, '|');
            
            // ID
            p = write_uint(num_buf, ids[i]);
            for (char* b = num_buf; b != p; ++b) hash = update_hash(hash, *b);
            
            // Separator
            hash = update_hash(hash, '|');
            
            // Level
            p = write_uint(num_buf, levels[i]);
            for (char* b = num_buf; b != p; ++b) hash = update_hash(hash, *b);
            
            // Separator
            hash = update_hash(hash, '|');
            
            // Message
            hash = update_hash_str(hash, messages[i]);
            
            // Newline
            hash = update_hash(hash, '\n');
        }
        final_hash = hash;
    }
    
    return final_hash;
}