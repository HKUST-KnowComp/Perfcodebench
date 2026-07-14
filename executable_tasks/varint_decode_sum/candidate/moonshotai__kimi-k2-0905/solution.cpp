#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const std::size_t len = encoded.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* p = data;
        const uint8_t* end = data + len;
        uint64_t sum = 0;

        while (p < end) {
            uint64_t val = 0;
            uint32_t shift = 0;
            const uint8_t* start = p;

            // unroll 4 bytes to reduce loop overhead and increase ILP
            if (p + 4 <= end) {
                uint64_t b0 = p[0];
                uint64_t b1 = p[1];
                uint64_t b2 = p[2];
                uint64_t b3 = p[3];
                val  = (b0 & 0x7F);
                if (b0 < 0x80) { p += 1; goto done; }
                val |= (b1 & 0x7F) << 7;
                if (b1 < 0x80) { p += 2; goto done; }
                val |= (b2 & 0x7F) << 14;
                if (b2 < 0x80) { p += 3; goto done; }
                val |= (b3 & 0x7F) << 21;
                if (b3 < 0x80) { p += 4; goto done; }
                p += 4;
                shift = 28;
            }

            // finish with a simple byte-at-a-time loop
            for (;;) {
                uint64_t byte = *p++;
                val |= (byte & 0x7F) << shift;
                if (byte < 0x80) break;
                shift += 7;
            }
done:
            sum += val;
        }
        total = sum;   // last iteration’s sum is the required result
    }
    return total;
}