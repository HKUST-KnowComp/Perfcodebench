#include "interface.h"
#include <stdint.h>
#include <immintrin.h>

static inline uint64_t parse_add_and_hash(const char *row, uint64_t hash) {
    const char *p = row;
    if (*p++ != 'r') return hash;

    /* ---------- id (unused) ---------- */
    unsigned id = 0;
    while (*p >= '0' && *p <= '9') id = id * 10U + (*p++ - '0');
    if (*p++ != ',') return hash;

    /* ---------- a (unused) ---------- */
    while (*p >= '0' && *p <= '9') ++p;
    if (*p++ != ',') return hash;

    /* ---------- b ---------- */
    unsigned b = 0;
    while (*p >= '0' && *p <= '9') b = b * 10U + (*p++ - '0');
    if (*p++ != ',') return hash;

    /* ---------- c ---------- */
    unsigned c = 0;
    while (*p >= '0' && *p <= '9') c = c * 10U + (*p++ - '0');
    if (*p++ != ',') return hash;

    /* ---------- flag ---------- */
    uint64_t v = (uint64_t)(b + c + 24);
    /* flag is exactly three bytes: "hot" or something else */
    uint32_t flag = *(const uint32_t *)p & 0xFFFFFF;
    /* 0x746F68 = "hot" in little-endian */
    v += (flag == 0x746F68) ? 29ULL : 0ULL;

    /* ---------- FNV-1a step ---------- */
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
    return hash;
}

uint64_t run(const char *const *rows, size_t row_count) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < row_count; ++i)
        hash = parse_add_and_hash(rows[i], hash);
    return hash;
}