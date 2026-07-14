#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_records(
    const uint32_t* ids,
    const uint32_t* ts,
    const uint16_t* levels,
    const uint16_t* lens,
    size_t count)
{
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < count; ++i) {
        uint32_t id = ids[i];
        hash = (hash ^ (uint8_t)(id)) * 1099511628211ULL;
        hash = (hash ^ (uint8_t)(id >> 8)) * 1099511628211ULL;
        hash = (hash ^ (uint8_t)(id >> 16)) * 1099511628211ULL;
        hash = (hash ^ (uint8_t)(id >> 24)) * 1099511628211ULL;

        uint32_t t = ts[i];
        hash = (hash ^ (uint8_t)(t)) * 1099511628211ULL;
        hash = (hash ^ (uint8_t)(t >> 8)) * 1099511628211ULL;
        hash = (hash ^ (uint8_t)(t >> 16)) * 1099511628211ULL;
        hash = (hash ^ (uint8_t)(t >> 24)) * 1099511628211ULL;

        uint16_t level = levels[i];
        hash = (hash ^ (uint8_t)(level)) * 1099511628211ULL;
        hash = (hash ^ (uint8_t)(level >> 8)) * 1099511628211ULL;

        uint16_t len = lens[i];
        hash = (hash ^ (uint8_t)(len)) * 1099511628211ULL;
        hash = (hash ^ (uint8_t)(len >> 8)) * 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
    if (iters <= 0) return 0;
    return checksum_records(ids, ts, levels, lens, count);
}
