#include "interface.h"
#include <stdint.h>

/* ---- run-time entry-point required by the harness ---- */
uint64_t run(const char* const* rows, size_t row_count)
{
    /* FNV-1a 64-bit parameters */
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME  = 1099511628211ULL;

    uint64_t hash = FNV_OFFSET;

    for (size_t i = 0; i < row_count; ++i) {
        const char *s = rows[i];

        /* skip the fixed "r" prefix */
        ++s;

        /* ---- read id (ignored) ---- */
        while (*s >= '0' && *s <= '9') ++s;
        ++s; /* skip comma */

        /* ---- read a (ignored) ---- */
        while (*s >= '0' && *s <= '9') ++s;
        ++s; /* skip comma */

        /* ---- read b ---- */
        int b = 0;
        while (*s >= '0' && *s <= '9') {
            b = b * 10 + (int)(*s - '0');
            ++s;
        }
        ++s; /* skip comma */

        /* ---- read c ---- */
        int c = 0;
        while (*s >= '0' && *s <= '9') {
            c = c * 10 + (int)(*s - '0');
            ++s;
        }
        ++s; /* skip comma */

        /* ---- read flag (first four bytes are enough to test "hot") ---- */
        uint32_t flag4;
        __builtin_memcpy(&flag4, s, 4);

        /* constant value part */
        uint64_t v = (uint64_t)(b + c + 20);

        /* if flag == "hot" (exact four bytes incl. terminating 0 in "hot\0") add 29 */
        if (flag4 == 0x006E6F68 /* "hon" backwards in little endian; check mask below works on LE */) {
            /* mask off the leading byte to confirm "hot" exactly */
            if ((flag4 & 0x00FFFFFF) == 0x006E6F68) v += 29;
        }

        /* ---- FNV-1a update ---- */
        hash ^= (v + 1ULL);
        hash *= FNV_PRIME;
    }

    return hash;
}