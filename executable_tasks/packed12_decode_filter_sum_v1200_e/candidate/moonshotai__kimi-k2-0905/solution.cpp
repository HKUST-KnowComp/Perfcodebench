#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed,
                               uint16_t threshold,
                               int iters) {
    const uint8_t* __restrict p = packed.data();
    const std::size_t n         = packed.size();
    const uint32_t thresh32     = threshold;          // hoist to 32-bit once

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        std::size_t i  = 0;

        // Process 24 bytes (16 values) per iteration
        for (; i + 24 <= n; i += 24) {
            // Load 24 bytes
            uint32_t w0, w1, w2, w3, w4, w5, w6, w7;
            w0 = *reinterpret_cast<const uint32_t*>(p + i + 0);
            w1 = *reinterpret_cast<const uint32_t*>(p + i + 4);
            w2 = *reinterpret_cast<const uint32_t*>(p + i + 8);
            w3 = *reinterpret_cast<const uint32_t*>(p + i + 12);
            w4 = *reinterpret_cast<const uint32_t*>(p + i + 16);
            w5 = *reinterpret_cast<const uint32_t*>(p + i + 20);

            // Decode 16 12-bit values
            uint32_t v0  = (w0 >>  0) & 0xFFFu;
            uint32_t v1  = (w0 >> 12) & 0xFFFu;
            uint32_t v2  = ((w0 >> 24) | (w1 << 8)) & 0xFFFu;
            uint32_t v3  = (w1 >>  4) & 0xFFFu;
            uint32_t v4  = (w1 >> 16) & 0xFFFu;
            uint32_t v5  = ((w1 >> 28) | (w2 << 4)) & 0xFFFu;
            uint32_t v6  = (w2 >>  8) & 0xFFFu;
            uint32_t v7  = (w2 >> 20) & 0xFFFu;
            uint32_t v8  = (w3 >>  0) & 0xFFFu;
            uint32_t v9  = (w3 >> 12) & 0xFFFu;
            uint32_t v10 = ((w3 >> 24) | (w4 << 8)) & 0xFFFu;
            uint32_t v11 = (w4 >>  4) & 0xFFFu;
            uint32_t v12 = (w4 >> 16) & 0xFFFu;
            uint32_t v13 = ((w4 >> 28) | (w5 << 4)) & 0xFFFu;
            uint32_t v14 = (w5 >>  8) & 0xFFFu;
            uint32_t v15 = (w5 >> 20) & 0xFFFu;

            // Filter and accumulate
            if (v0  > thresh32) local += v0;
            if (v1  > thresh32) local += v1;
            if (v2  > thresh32) local += v2;
            if (v3  > thresh32) local += v3;
            if (v4  > thresh32) local += v4;
            if (v5  > thresh32) local += v5;
            if (v6  > thresh32) local += v6;
            if (v7  > thresh32) local += v7;
            if (v8  > thresh32) local += v8;
            if (v9  > thresh32) local += v9;
            if (v10 > thresh32) local += v10;
            if (v11 > thresh32) local += v11;
            if (v12 > thresh32) local += v12;
            if (v13 > thresh32) local += v13;
            if (v14 > thresh32) local += v14;
            if (v15 > thresh32) local += v15;
        }

        // Handle remaining bytes (0..23)
        for (; i + 3 <= n; i += 3) {
            uint32_t a = p[i] | ((p[i + 1] & 0x0Fu) << 8);
            uint32_t b = (p[i + 1] >> 4) | (p[i + 2] << 4);
            if (a > thresh32) local += a;
            if (b > thresh32) local += b;
        }
        sum = local;
    }
    return sum;
}