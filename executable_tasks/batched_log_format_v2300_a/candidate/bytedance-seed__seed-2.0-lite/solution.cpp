#include "interface.h"
#include <string>
#include <vector>
#include <charconv>
#include <cstdint>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    const size_t num_entries = ids.size();
    size_t total_cap = 0;
    // Precompute worst-case total size to reserve output buffer once
    for (size_t i = 0; i < num_entries; ++i) {
        total_cap += messages[i].size() + 29;
    }
    std::string out;
    out.reserve(total_cap);
    uint64_t final_hash = 0;
    constexpr uint64_t FNV1A_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV1A_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        uint64_t hash = FNV1A_OFFSET;
        char num_buf[32];

        for (size_t i = 0; i < num_entries; ++i) {
            // Write timestamp
            auto [ts_end, _] = std::to_chars(num_buf, num_buf + 32, timestamps[i]);
            size_t ts_len = ts_end - num_buf;
            for (size_t j = 0; j < ts_len; ++j) {
                const unsigned char c = static_cast<unsigned char>(num_buf[j]);
                out.push_back(c);
                hash ^= static_cast<uint64_t>(c);
                hash *= FNV1A_PRIME;
            }
            // Write separator
            const unsigned char sep = static_cast<unsigned char>('|');
            out.push_back(sep);
            hash ^= static_cast<uint64_t>(sep);
            hash *= FNV1A_PRIME;

            // Write ID
            auto [id_end, __] = std::to_chars(num_buf, num_buf + 32, ids[i]);
            size_t id_len = id_end - num_buf;
            for (size_t j = 0; j < id_len; ++j) {
                const unsigned char c = static_cast<unsigned char>(num_buf[j]);
                out.push_back(c);
                hash ^= static_cast<uint64_t>(c);
                hash *= FNV1A_PRIME;
            }
            out.push_back(sep);
            hash ^= static_cast<uint64_t>(sep);
            hash *= FNV1A_PRIME;

            // Write log level
            auto [lvl_end, ___] = std::to_chars(num_buf, num_buf + 32, levels[i]);
            size_t lvl_len = lvl_end - num_buf;
            for (size_t j = 0; j < lvl_len; ++j) {
                const unsigned char c = static_cast<unsigned char>(num_buf[j]);
                out.push_back(c);
                hash ^= static_cast<uint64_t>(c);
                hash *= FNV1A_PRIME;
            }
            out.push_back(sep);
            hash ^= static_cast<uint64_t>(sep);
            hash *= FNV1A_PRIME;

            // Write log message
            for (char raw_c : messages[i]) {
                const unsigned char c = static_cast<unsigned char>(raw_c);
                out.push_back(c);
                hash ^= static_cast<uint64_t>(c);
                hash *= FNV1A_PRIME;
            }
            // Write newline
            const unsigned char nl = static_cast<unsigned char>('\n');
            out.push_back(nl);
            hash ^= static_cast<uint64_t>(nl);
            hash *= FNV1A_PRIME;
        }
        final_hash = hash;
    }
    return final_hash;
}