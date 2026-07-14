#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>

namespace {

// FNV-1a 64-bit constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Fast integer to string conversion that updates the hash directly
inline void hash_uint(uint64_t& hash, uint32_t val) {
    if (val == 0) {
        hash ^= static_cast<uint64_t>('0');
        hash *= FNV_PRIME;
        return;
    }
    char buf[11];
    int pos = 11;
    while (val > 0) {
        buf[--pos] = (val % 10) + '0';
        val /= 10;
    }
    for (int i = pos; i < 11; ++i) {
        hash ^= static_cast<uint64_t>(buf[i]);
        hash *= FNV_PRIME;
    }
}

inline void hash_char(uint64_t& hash, char c) {
    hash ^= static_cast<uint64_t>(c);
    hash *= FNV_PRIME;
}

inline void hash_string(uint64_t& hash, const std::string& s) {
    for (char c : s) {
        hash ^= static_cast<uint64_t>(c);
        hash *= FNV_PRIME;
    }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    uint64_t final_hash = 0;
    size_t n = ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < n; ++i) {
            hash_uint(current_hash, timestamps[i]);
            hash_char(current_hash, '|');
            hash_uint(current_hash, ids[i]);
            hash_char(current_hash, '|');
            hash_uint(current_hash, levels[i]);
            hash_char(current_hash, '|');
            hash_string(current_hash, messages[i]);
            hash_char(current_hash, '\n');
        }
        final_hash = current_hash;
    }
    
    return final_hash;
}