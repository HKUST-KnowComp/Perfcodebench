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

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
    size_t total_bytes = count * sizeof(Record);
    uint8_t* out = (uint8_t*)malloc(total_bytes);
    if (!out) exit(4);

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        Record* ptr = (Record*)out;
        for (size_t i = 0; i < count; ++i) {
            ptr[i].id = ids[i];
            ptr[i].ts = ts[i];
            ptr[i].level = levels[i];
            ptr[i].len = lens[i];
        }

        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_bytes; ++i) {
            hash ^= (uint64_t)out[i];
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    free(out);
    return final_hash;
}