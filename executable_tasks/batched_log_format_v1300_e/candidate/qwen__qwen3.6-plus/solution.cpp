#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void append_u32(uint32_t v, uint64_t& h) {
    if (v == 0) { h ^= '0'; h *= FNV_PRIME; return; }
    char buf[10]; int p = 10;
    while (v) { buf[--p] = '0' + (v % 10); v /= 10; }
    for (int i = p; i < 10; ++i) { h ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[i])); h *= FNV_PRIME; }
}

inline void append_u16(uint16_t v, uint64_t& h) {
    if (v == 0) { h ^= '0'; h *= FNV_PRIME; return; }
    char buf[5]; int p = 5;
    while (v) { buf[--p] = '0' + (v % 10); v /= 10; }
    for (int i = p; i < 5; ++i) { h ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[i])); h *= FNV_PRIME; }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    uint64_t hash = 0;
    const size_t n = ids.size();
    const uint64_t P = FNV_PRIME;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            append_u32(timestamps[i], hash);
            hash ^= static_cast<uint64_t>('|'); hash *= P;
            append_u32(ids[i], hash);
            hash ^= static_cast<uint64_t>('|'); hash *= P;
            append_u16(levels[i], hash);
            hash ^= static_cast<uint64_t>('|'); hash *= P;
            const std::string& msg = messages[i];
            for (char c : msg) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
                hash *= P;
            }
            hash ^= static_cast<uint64_t>('\n'); hash *= P;
        }
    }
    return hash;
}
