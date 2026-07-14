#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    size_t i = 0;
    // Unroll loop to process 8 bytes at a time to improve instruction-level parallelism
    // Note: FNV-1a is byte-oriented, so we must maintain the byte-wise XOR logic.
    // However, we can unroll the loop to reduce branch overhead.
    for (; i + 7 < n; i += 8) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
        hash ^= (uint64_t)data[i+1];
        hash *= prime;
        hash ^= (uint64_t)data[i+2];
        hash *= prime;
        hash ^= (uint64_t)data[i+3];
        hash *= prime;
        hash ^= (uint64_t)data[i+4];
        hash *= prime;
        hash ^= (uint64_t)data[i+5];
        hash *= prime;
        hash ^= (uint64_t)data[i+6];
        hash *= prime;
        hash ^= (uint64_t)data[i+7];
        hash *= prime;
    }

    for (; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
    }
    return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
    uint64_t hash = 0;
    size_t record_size = 12;
    size_t total_size = count * record_size;
    
    // Pre-allocate the entire buffer once to avoid realloc overhead
    uint8_t* out = (uint8_t*)malloc(total_size);
    if (!out) exit(4);

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = out;
        for (size_t i = 0; i < count; ++i) {
            // Manual serialization to ensure little-endian layout as per baseline
            // Baseline: ids[i] (4), ts[i] (4), levels[i] (2), lens[i] (2)
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t l = levels[i];
            uint16_t ln = lens[i];

            // Using memcpy or direct assignment is faster than push_byte
            // We use direct assignment to ensure exact byte order matching the baseline
            *ptr++ = (uint8_t)(id & 0xFF);
            *ptr++ = (uint8_t)((id >> 8) & 0xFF);
            *ptr++ = (uint8_t)((id >> 16) & 0xFF);
            *ptr++ = (uint8_t)((id >> 24) & 0xFF);

            *ptr++ = (uint8_t)(t & 0xFF);
            *ptr++ = (uint8_t)((t >> 8) & 0xFF);
            *ptr++ = (uint8_t)((t >> 16) & 0xFF);
            *ptr++ = (uint8_t)((t >> 24) & 0xFF);

            *ptr++ = (uint8_t)(l & 0xFF);
            *ptr++ = (uint8_t)((l >> 8) & 0xFF);

            *ptr++ = (uint8_t)(ln & 0xFF);
            *ptr++ = (uint8_t)((ln >> 8) & 0xFF);
        }
        hash = checksum_bytes(out, total_size);
    }

    free(out);
    return hash;
}