#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

namespace {

// Fast integer to string conversion for uint32_t
inline char* write_uint(char* buf, uint32_t val) {
    if (val == 0) {
        *buf++ = '0';
        return buf;
    }
    char temp[10];
    int i = 10;
    while (val > 0) {
        temp[--i] = (val % 10) + '0';
        val /= 10;
    }
    std::memcpy(buf, &temp[i], 10 - i);
    return buf + (10 - i);
}

// FNV-1a constants
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t update_hash(uint64_t hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= FNV_PRIME;
    }
    return hash;
}

inline uint64_t update_hash_char(uint64_t hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= FNV_PRIME;
    return hash;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    size_t n = ids.size();
    uint64_t final_hash = 0;
    
    // Reusable buffer for integer formatting to avoid allocations
    char int_buf[16];

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < n; ++i) {
            // Timestamp
            char* end = write_uint(int_buf, timestamps[i]);
            hash = update_hash(hash, int_buf, end - int_buf);
            hash = update_hash_char(hash, '|');
            
            // ID
            end = write_uint(int_buf, ids[i]);
            hash = update_hash(hash, int_buf, end - int_buf);
            hash = update_hash_char(hash, '|');
            
            // Level
            end = write_uint(int_buf, static_cast<uint32_t>(levels[i]));
            hash = update_hash(hash, int_buf, end - int_buf);
            hash = update_hash_char(hash, '|');
            
            // Message
            hash = update_hash(hash, messages[i].data(), messages[i].size());
            hash = update_hash_char(hash, '\n');
        }
        final_hash = hash;
    }
    
    return final_hash;
}