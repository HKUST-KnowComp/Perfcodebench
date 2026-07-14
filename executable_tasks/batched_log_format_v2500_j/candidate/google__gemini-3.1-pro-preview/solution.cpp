#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>

namespace {

inline void hash_update_char(uint64_t& hash, char c) {
    hash ^= static_cast<uint8_t>(c);
    hash *= 1099511628211ULL;
}

inline void hash_update_int(uint64_t& hash, uint32_t v) {
    if (v == 0) {
        hash_update_char(hash, '0');
        return;
    }
    char buf[16];
    char* p = buf + 16;
    while (v >= 10) {
        uint32_t q = v / 10;
        uint32_t r = v % 10;
        *--p = '0' + r;
        v = q;
    }
    *--p = '0' + v;
    while (p < buf + 16) {
        hash_update_char(hash, *p++);
    }
}

inline void hash_update_str(uint64_t& hash, const std::string& s) {
    for (char c : s) {
        hash ^= static_cast<uint8_t>(c);
        hash *= 1099511628211ULL;
    }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (iters <= 0) return 0;

    uint64_t hash = 1469598103934665603ULL;
    std::size_t n = ids.size();
    for (std::size_t i = 0; i < n; ++i) {
        hash_update_int(hash, timestamps[i]);
        hash_update_char(hash, '|');
        hash_update_int(hash, ids[i]);
        hash_update_char(hash, '|');
        hash_update_int(hash, levels[i]);
        hash_update_char(hash, '|');
        hash_update_str(hash, messages[i]);
        hash_update_char(hash, '\n');
    }

    return hash;
}
