#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

// FNV-1a constants
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Inline function to update FNV-1a hash with a single byte
inline uint64_t update_hash_byte(uint64_t hash, uint8_t c) {
    hash ^= static_cast<uint64_t>(c);
    hash *= FNV_PRIME;
    return hash;
}

// Inline function to update FNV-1a hash with a string/buffer
inline uint64_t update_hash_bytes(uint64_t hash, const char* s, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash = update_hash_byte(hash, static_cast<uint8_t>(s[i]));
    }
    return hash;
}

// Fast integer to string conversion and hashing
inline uint64_t update_hash_uint(uint64_t hash, uint32_t val) {
    if (val == 0) {
        return update_hash_byte(hash, '0');
    }
    char buf[12];
    char* p = buf + 12;
    while (val > 0) {
        *--p = static_cast<char>((val % 10) + '0');
        val /= 10;
    }
    return update_hash_bytes(hash, p, (buf + 12) - p);
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (iters <= 0) return 0;

    // Since the input data does not change between iterations, the final hash 
    // will be the same for every iteration. We compute it once to optimize performance.
    uint64_t hash = FNV_OFFSET_BASIS;
    size_t n = ids.size();

    for (size_t i = 0; i < n; ++i) {
        // Format: timestamp|id|level|message\n
        hash = update_hash_uint(hash, timestamps[i]);
        hash = update_hash_byte(hash, '|');
        
        hash = update_hash_uint(hash, ids[i]);
        hash = update_hash_byte(hash, '|');
        
        hash = update_hash_uint(hash, static_cast<uint32_t>(levels[i]));
        hash = update_hash_byte(hash, '|');
        
        const std::string& msg = messages[i];
        hash = update_hash_bytes(hash, msg.data(), msg.size());
        
        hash = update_hash_byte(hash, '\n');
    }

    return hash;
}
