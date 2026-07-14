#include "interface.h"

#include <string>
#include <vector>
#include <charconv>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Fast update for the hash
inline void update_hash(uint64_t& hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= FNV_PRIME;
}

inline void update_hash_str(uint64_t& hash, const std::string& s) {
    for (char c : s) {
        update_hash(hash, c);
    }
}

inline void update_hash_int(uint64_t& hash, uint32_t val) {
    if (val == 0) {
        update_hash(hash, '0');
        return;
    }
    char buf[11];
    char* ptr = buf + 10;
    *ptr = '\0';
    while (val > 0) {
        *(--ptr) = '0' + (val % 10);
        val /= 10;
    }
    for (char* p = ptr; *p != '\0'; ++p) {
        update_hash(hash, *p);
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
    const size_t n = ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < n; ++i) {
            update_hash_int(hash, timestamps[i]);
            update_hash(hash, '|');
            update_hash_int(hash, ids[i]);
            update_hash(hash, '|');
            update_hash_int(hash, levels[i]);
            update_hash(hash, '|');
            update_hash_str(hash, messages[i]);
            update_hash(hash, '\n');
        }
        final_hash = hash;
    }
    
    return final_hash;
}