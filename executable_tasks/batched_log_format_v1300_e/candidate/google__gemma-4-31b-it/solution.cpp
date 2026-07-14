#include "interface.h"

#include <string>
#include <vector>
#include <charconv>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void update_hash(uint64_t& hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= FNV_PRIME;
}

inline void update_hash_int(uint64_t& hash, uint32_t val) {
    char buf[11];
    auto [ptr, ec] = std::to_chars(buf, buf + 11, val);
    for (char* p = buf; p != ptr; ++p) {
        update_hash(hash, *p);
    }
}

inline void update_hash_int16(uint64_t& hash, uint16_t val) {
    char buf[6];
    auto [ptr, ec] = std::to_chars(buf, buf + 6, val);
    for (char* p = buf; p != ptr; ++p) {
        update_hash(hash, *p);
    }
}

inline void update_hash_string(uint64_t& hash, const std::string& s) {
    for (char c : s) {
        update_hash(hash, c);
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
            update_hash_int16(current_hash, levels[i]);
            update_hash(current_hash, '|');
            update_hash_string(current_hash, messages[i]);
            update_hash(current_hash, '\n');
        }
        final_hash = current_hash;
    }
    
    return final_hash;
}