#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
    uint32_t id;
    uint32_t ts;
    uint16_t level;
    uint16_t len;
} Record;
#pragma pack(pop)

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
    size_t total_bytes = count * sizeof(Record);
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        Record* ptr = (Record*)out;
        for (size_t i = 0; i < count; ++i) {
            ptr[i].id = ids[i];
            ptr[i].ts = ts[i];
            ptr[i].level = levels[i];
            ptr[i].len = lens[i];
        }
        hash = checksum_bytes(out, total_bytes);
    }

    free(out);
    return hash;
}