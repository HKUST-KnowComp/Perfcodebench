#include "interface.h"

#include <string>
#include <vector>
#include <charconv>

namespace {

// FNV-1a 64-bit constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void update_hash(uint64_t& hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= FNV_PRIME;
}

inline void update_hash_int(uint64_t& hash, uint32_t val) {
    char buf[11];
    auto [ptr, ec] = std::to_chars(buf, buf + 11, val);
    for (char* p = buf; p < ptr; ++p) {
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
    size_t n = ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = FNV_OFFSET_BASIS;
        
        for (size_t i = 0; i < n; ++i) {
            update_hash_int(current_hash, timestamps[i]);
            update_hash(current_hash, '|');
            update_hash_int(current_hash, ids[i]);
            update_hash(current_hash, '|');
            update_hash_int(current_hash, levels[i]);
            update_hash(current_hash, '|');
            
            const std::string& msg = messages[i];
            for (char c : msg) {
                update_hash(current_hash, c);
            }
            update_hash(current_hash, '\n');
        }
        final_hash = current_hash;
    }
    
    return final_hash;
}