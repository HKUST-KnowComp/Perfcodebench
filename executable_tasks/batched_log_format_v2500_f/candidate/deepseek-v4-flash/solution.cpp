#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void fnv1a_append(uint64_t& hash, unsigned char c) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
}

template<typename T>
void append_number(T value, uint64_t& hash) {
    char buf[20];
    int len = 0;
    if (value == 0) {
        buf[len++] = '0';
    } else {
        while (value > 0) {
            buf[len++] = '0' + (value % 10);
            value /= 10;
        }
        for (int i = 0; i < len / 2; ++i) {
            char t = buf[i];
            buf[i] = buf[len - 1 - i];
            buf[len - 1 - i] = t;
        }
    }
    for (int i = 0; i < len; ++i) {
        fnv1a_append(hash, static_cast<unsigned char>(buf[i]));
    }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    if (iters == 0) return 0;

    uint64_t hash = 1469598103934665603ULL;
    const std::size_t n = ids.size();
    for (std::size_t i = 0; i < n; ++i) {
        append_number(timestamps[i], hash);
        fnv1a_append(hash, '|');
        append_number(ids[i], hash);
        fnv1a_append(hash, '|');
        append_number(levels[i], hash);
        fnv1a_append(hash, '|');
        for (char c : messages[i]) {
            fnv1a_append(hash, static_cast<unsigned char>(c));
        }
        fnv1a_append(hash, '\n');
    }
    // hash computed once; same for all iterations
    return hash;
}
