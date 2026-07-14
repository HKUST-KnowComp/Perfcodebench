#include "interface.h"

#include <string>
#include <vector>
#include <algorithm>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void append_uint(std::string& out, uint64_t n, uint64_t& hash) {
    if (n == 0) {
        const unsigned char c = '0';
        out += c;
        hash ^= static_cast<uint64_t>(c);
        hash *= FNV_PRIME;
        return;
    }
    char buf[16];
    char* p = buf;
    while (n > 0) {
        *p++ = '0' + (n % 10);
        n /= 10;
    }
    std::reverse(buf, p);
    for (char* c = buf; c < p; ++c) {
        const unsigned char uc = static_cast<unsigned char>(*c);
        out += uc;
        hash ^= static_cast<uint64_t>(uc);
        hash *= FNV_PRIME;
    }
}

inline void append_char(std::string& out, char c, uint64_t& hash) {
    const unsigned char uc = static_cast<unsigned char>(c);
    out += uc;
    hash ^= static_cast<uint64_t>(uc);
    hash *= FNV_PRIME;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    size_t total_capacity = 0;
    const size_t num_entries = ids.size();
    for (const auto& msg : messages) {
        total_capacity += msg.size();
    }
    total_capacity += num_entries * 32;
    std::string out;
    out.reserve(total_capacity);

    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        uint64_t hash = FNV_OFFSET;
        for (size_t i = 0; i < num_entries; ++i) {
            append_uint(out, timestamps[i], hash);
            append_char(out, '|', hash);
            append_uint(out, ids[i], hash);
            append_char(out, '|', hash);
            append_uint(out, levels[i], hash);
            append_char(out, '|', hash);
            for (char mc : messages[i]) {
                append_char(out, mc, hash);
            }
            append_char(out, '\n', hash);
        }
        final_hash = hash;
    }
    return final_hash;
}