#include "interface.h"

#include <string>
#include <vector>

namespace {

inline void fnv_update(uint64_t& hash, char c) noexcept {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= 1099511628211ULL;
}

template<typename T>
inline void hash_integer(uint64_t& hash, T val) {
    char buf[20]; // enough for any unsigned integer up to 64 bits
    char* p = buf + sizeof(buf);
    if (val == 0) {
        *(--p) = '0';
    } else {
        while (val > 0) {
            *(--p) = static_cast<char>('0' + (val % 10));
            val /= 10;
        }
    }
    while (p != buf + sizeof(buf)) {
        fnv_update(hash, *p++);
    }
}

} // anonymous namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    uint64_t hash = 0;
    const std::size_t n = ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (std::size_t i = 0; i < n; ++i) {
            hash_integer(h, timestamps[i]);
            fnv_update(h, '|');
            hash_integer(h, ids[i]);
            fnv_update(h, '|');
            hash_integer(h, levels[i]);
            fnv_update(h, '|');
            const std::string& msg = messages[i];
            for (char c : msg) {
                fnv_update(h, c);
            }
            fnv_update(h, '\n');
        }
        hash = h;
    }
    return hash;
}
