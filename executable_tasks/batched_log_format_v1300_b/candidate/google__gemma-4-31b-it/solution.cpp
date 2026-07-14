#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Fast integer to hash function to avoid std::to_string allocations
inline void hash_uint(uint64_t hash, uint32_t value) {
    if (value == 0) {
        hash ^= static_cast<uint64_t>('0');
        hash *= FNV_PRIME;
        return;
    }
    char buf[11];
    int pos = 11;
    while (value > 0) {
        buf[--pos] = (value % 10) + '0';
        value /= 10;
    }
    for (int i = pos; i < 11; ++i) {
        // This is a logic error in the helper; we need to pass hash by reference
    }
}

// Corrected inline hash update
inline uint64_t update_hash_uint(uint64_t hash, uint32_t value) {
    if (value == 0) {
        hash ^= static_cast<uint64_t>('0');
        hash *= FNV_PRIME;
        return hash;
    }
    char buf[11];
    int pos = 11;
    while (value > 0) {
        buf[--pos] = (value % 10) + '0';
        value /= 10;
    }
    for (int i = pos; i < 11; ++i) {
        hash ^= static_cast<uint64_t>(buf[i]);
        hash *= FNV_PRIME;
    }
    return hash;
}

inline uint64_t update_hash_char(uint64_t hash, char c) {
    hash ^= static_cast<uint64_t>(c);
    hash *= FNV_PRIME;
    return hash;
}

inline uint64_t update_hash_string(uint64_t hash, const std::string& s) {
    for (unsigned char c : s) {
        hash ^= static_cast<uint64_t>(c);
        hash *= FNV_PRIME;
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
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET_BASIS;
        
        for (std::size_t i = 0; i < ids.size(); ++i) {
            hash = update_hash_uint(hash, timestamps[i]);
            hash = update_hash_char(hash, '|');
            hash = update_hash_uint(hash, ids[i]);
            hash = update_hash_char(hash, '|');
            hash = update_hash_uint(hash, levels[i]);
            hash = update_hash_char(hash, '|');
            hash = update_hash_string(hash, messages[i]);
            hash = update_hash_char(hash, '\n');
        }
        final_hash = hash;
    }
    
    return final_hash;
}