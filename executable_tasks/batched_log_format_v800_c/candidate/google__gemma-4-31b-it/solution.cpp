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
    int pos = 0;
    while (val) {
        temp[pos++] = (val % 10) + '0';
        val /= 10;
    }
    while (pos > 0) {
        *buf++ = temp[--pos];
    }
    return buf;
}

// FNV-1a hash update
inline uint64_t update_hash(uint64_t hash, unsigned char c) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
    return hash;
}

// Process a string for the hash
inline uint64_t hash_string(uint64_t hash, const std::string& s) {
    for (unsigned char c : s) {
        hash = update_hash(hash, c);
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
    const size_t n = ids.size();
    char buf[32];

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        
        for (size_t i = 0; i < n; ++i) {
            // Timestamp
            char* p = buf;
            p = write_uint(p, timestamps[i]);
            for (char* c = buf; c < p; ++c) hash = update_hash(hash, *c);
            
            // Separator
            hash = update_hash(hash, '|');
            
            // ID
            p = buf;
            p = write_uint(p, ids[i]);
            for (char* c = buf; c < p; ++c) hash = update_hash(hash, *c);
            
            // Separator
            hash = update_hash(hash, '|');
            
            // Level
            p = buf;
            p = write_uint(p, levels[i]);
            for (char* c = buf; c < p; ++c) hash = update_hash(hash, *c);
            
            // Separator
            hash = update_hash(hash, '|');
            
            // Message
            hash = hash_string(hash, messages[i]);
            
            // Newline
            hash = update_hash(hash, '\n');
        }
        final_hash = hash;
    }
    
    return final_hash;
}