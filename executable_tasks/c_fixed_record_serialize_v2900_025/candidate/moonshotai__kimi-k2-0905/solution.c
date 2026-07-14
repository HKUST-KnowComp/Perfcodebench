#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
    const size_t rec_size = 12;
    const size_t total = count * rec_size;
    uint8_t* out = (uint8_t*)malloc(total);
    if (!out) exit(4);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* dst = out;
        for (size_t i = 0; i < count; ++i) {
            struct {
                uint32_t id;
                uint32_t ts;
                uint16_t level;
                uint16_t len;
            } __attribute__((packed)) rec = { ids[i], ts[i], levels[i], lens[i] };
            memcpy(dst, &rec, rec_size);
            dst += rec_size;
        }
        hash = checksum_bytes(out, total);
    }
    free(out);
    return hash;
}