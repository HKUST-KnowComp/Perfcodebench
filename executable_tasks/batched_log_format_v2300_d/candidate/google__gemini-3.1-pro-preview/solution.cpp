#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

namespace {

// Fast integer to string conversion for uint32_t
// Returns pointer to the start of the string in the buffer
inline char* fast_utoa(uint32_t value, char* buffer) {
    if (value == 0) {
        *--buffer = '0';
        return buffer;
    }
    while (value > 0) {
        *--buffer = (char)('0' + (value % 10));
        value /= 10;
    }
    return buffer;
}

// FNV-1a constants
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void fnv_update(uint64_t& hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
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
    
    // Temporary buffer for integer conversions (max uint32_t is 10 digits)
    char num_buf[12];
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = FNV_OFFSET_BASIS;
        
        for (size_t i = 0; i < n; ++i) {
            // Timestamp
            char* p = fast_utoa(timestamps[i], num_buf + 11);
            fnv_update(current_hash, p, (num_buf + 11) - p);
            
            // Separator
            current_hash ^= static_cast<uint64_t>('|');
            current_hash *= FNV_PRIME;
            
            // ID
            p = fast_utoa(ids[i], num_buf + 11);
            fnv_update(current_hash, p, (num_buf + 11) - p);
            
            // Separator
            current_hash ^= static_cast<uint64_t>('|');
            current_hash *= FNV_PRIME;
            
            // Level
            p = fast_utoa(levels[i], num_buf + 11);
            fnv_update(current_hash, p, (num_buf + 11) - p);
            
            // Separator
            current_hash ^= static_cast<uint64_t>('|');
            current_hash *= FNV_PRIME;
            
            // Message
            fnv_update(current_hash, messages[i].data(), messages[i].size());
            
            // Newline
            current_hash ^= static_cast<uint64_t>('\n');
            current_hash *= FNV_PRIME;
        }
        final_hash = current_hash;
    }
    
    return final_hash;
}