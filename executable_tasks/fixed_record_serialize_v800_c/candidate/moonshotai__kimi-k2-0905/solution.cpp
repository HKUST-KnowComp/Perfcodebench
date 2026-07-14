#include "interface.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

// FNV-1a parameters
static constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
static constexpr uint64_t FNV_PRIME        = 1099511628211ULL;

static inline uint64_t checksum_bytes(const uint8_t* ptr, size_t len)
{
    uint64_t h = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<uint64_t>(ptr[i]);
        h *= FNV_PRIME;
    }
    return h;
}

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters)
{
    const size_t n = ids.size();
    // Each record: 8 bytes payload (id+ts) + 4 bytes packed (levels<<16|lens)
    const size_t total = n * 12;

    // Single output buffer, pre-size once
    std::vector<uint8_t> buffer;
    buffer.resize(total);

    // Convert from bytes to 32-bit aligned pointer for fast word writes
    uint8_t* out = buffer.data();

    // First pass: build the entire buffer and compute base checksum
    uint8_t* p = out;
    for (size_t i = 0; i < n; ++i) {
        // write id (4 bytes)
        *reinterpret_cast<uint32_t*>(p) = ids[i];
        p += 4;
        // write ts (4 bytes)
        *reinterpret_cast<uint32_t*>(p) = ts[i];
        p += 4;
        // pack levels and lens into one 32-bit word
        uint32_t packed = (static_cast<uint32_t>(levels[i]) << 16) | lens[i];
        *reinterpret_cast<uint32_t*>(p) = packed;
        p += 4;
    }

    uint64_t h = checksum_bytes(out, total);

    // All further iterations reuse the same buffer; only need to re-calc per-round
    for (int iter = 1; iter < iters; ++iter) {
        h = checksum_bytes(out, total);
    }
    return h;
}