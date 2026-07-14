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

uint64_t run(const uint32_t* ids,
             const uint32_t* ts,
             const uint16_t* levels,
             const uint16_t* lens,
             size_t count,
             int iters)
{
    const size_t rec_size = 12;               /* 4 + 4 + 2 + 2 */
    const size_t total    = count * rec_size;

    uint8_t* buf = (uint8_t*)malloc(total);
    if (!buf) exit(4);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* p = buf;
        for (size_t i = 0; i < count; ++i) {
            uint32_t id   = ids[i];
            uint32_t t    = ts[i];
            uint16_t lvl  = levels[i];
            uint16_t ln   = lens[i];

            p[0] = (uint8_t)(id);
            p[1] = (uint8_t)(id >> 8);
            p[2] = (uint8_t)(id >> 16);
            p[3] = (uint8_t)(id >> 24);

            p[4] = (uint8_t)(t);
            p[5] = (uint8_t)(t >> 8);
            p[6] = (uint8_t)(t >> 16);
            p[7] = (uint8_t)(t >> 24);

            p[8]  = (uint8_t)(lvl);
            p[9]  = (uint8_t)(lvl >> 8);

            p[10] = (uint8_t)(ln);
            p[11] = (uint8_t)(ln >> 8);

            p += rec_size;
        }
        hash = checksum_bytes(buf, total);
    }
    free(buf);
    return hash;
}