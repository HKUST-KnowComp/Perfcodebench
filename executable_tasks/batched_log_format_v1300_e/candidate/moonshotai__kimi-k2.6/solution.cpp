#include "interface.h"

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void fnv1a_feed(uint64_t& hash, const char* data, std::size_t len) {
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
}

inline void fnv1a_feed_char(uint64_t& hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= 1099511628211ULL;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    if (iters <= 0) {
        return 0;
    }

    uint64_t hash = 1469598103934665603ULL;
    const std::size_t n = ids.size();
    char buf[32];

    for (std::size_t i = 0; i < n; ++i) {
        auto r1 = std::to_chars(buf, buf + sizeof(buf), timestamps[i]);
        fnv1a_feed(hash, buf, static_cast<std::size_t>(r1.ptr - buf));
        fnv1a_feed_char(hash, '|');

        auto r2 = std::to_chars(buf, buf + sizeof(buf), ids[i]);
        fnv1a_feed(hash, buf, static_cast<std::size_t>(r2.ptr - buf));
        fnv1a_feed_char(hash, '|');

        auto r3 = std::to_chars(buf, buf + sizeof(buf), static_cast<uint32_t>(levels[i]));
        fnv1a_feed(hash, buf, static_cast<std::size_t>(r3.ptr - buf));
        fnv1a_feed_char(hash, '|');

        const std::string& msg = messages[i];
        fnv1a_feed(hash, msg.data(), msg.size());
        fnv1a_feed_char(hash, '\n');
    }

    return hash;
}
