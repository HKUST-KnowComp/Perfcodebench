#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    // Unroll the loop to reduce branch overhead and improve instruction-level parallelism
    size_t i = 0;
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
    const size_t record_size = 12;
    const size_t total_size = count * record_size;
    uint64_t hash = 0;
    
    // Pre-allocate the entire buffer once to avoid realloc overhead
    uint8_t* out = (uint8_t*)malloc(total_size);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = out;
        for (size_t i = 0; i < count; ++i) {
            // Manually unroll the serialization to match the exact little-endian byte order required
            // Record: ID (4), TS (4), Level (2), Len (2) = 12 bytes
            
            uint32_t id = ids[i];
            ptr[0] = (uint8_t)id;
            ptr[1] = (uint8_t)(id >> 8);
            ptr[2] = (uint8_t)(id >> 16);
            ptr[3] = (uint8_t)(id >> 24);

            uint32_t t = ts[i];
            ptr[4] = (uint8_t)t;
            ptr[5] = (uint8_t)(t >> 8);
            ptr[6] = (uint8_t)(t >> 16);
            ptr[7] = (uint8_t)(t >> 24);

            uint16_t lvl = levels[i];
            ptr[8] = (uint8_t)lvl;
            ptr[9] = (uint8_t)(lvl >> 8);

            uint16_t ln = lens[i];
            ptr[10] = (uint8_t)ln;
            ptr[11] = (uint8_t)(ln >> 8);

            ptr += record_size;
        }
        hash = checksum_bytes(out, total_size);
    }

    free(out);
    return hash;
}