#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch,
                              int max_len,
                              int iters)
{
    // Pre-compute total packed length once; reuse buffers across iterations.
    std::size_t total = 0;
    for (int b = 0; b < batch; ++b) total += static_cast<std::size_t>(lengths[static_cast<std::size_t>(b)]);

    std::vector<uint32_t> packed_tokens(total);
    std::vector<uint32_t> positions(total);

    uint64_t hash = 14695981039346656037ULL;

    for (int iter = 0; iter < iters; ++iter) {
        // Single fused pass: pack + build positions + hash on the fly.
        const uint32_t* src = padded_tokens.data();
        uint32_t* dst_tok = packed_tokens.data();
        uint32_t* dst_pos = positions.data();
        uint64_t h = 14695981039346656037ULL;

        for (int b = 0; b < batch; ++b) {
            int len = lengths[static_cast<std::size_t>(b)];
            // Unroll inner loop 4× to reduce branch overhead and hide latency.
            int i = 0;
            for (; i + 3 < len; i += 4) {
                uint32_t t0 = src[i + 0];
                uint32_t t1 = src[i + 1];
                uint32_t t2 = src[i + 2];
                uint32_t t3 = src[i + 3];

                dst_tok[0] = t0; dst_pos[0] = static_cast<uint32_t>(i + 0);
                dst_tok[1] = t1; dst_pos[1] = static_cast<uint32_t>(i + 1);
                dst_tok[2] = t2; dst_pos[2] = static_cast<uint32_t>(i + 2);
                dst_tok[3] = t3; dst_pos[3] = static_cast<uint32_t>(i + 3);

                h = mix(h, t0); h = mix(h, static_cast<uint32_t>(i + 0));
                h = mix(h, t1); h = mix(h, static_cast<uint32_t>(i + 1));
                h = mix(h, t2); h = mix(h, static_cast<uint32_t>(i + 2));
                h = mix(h, t3); h = mix(h, static_cast<uint32_t>(i + 3));

                dst_tok += 4;
                dst_pos += 4;
            }
            // Tail
            for (; i < len; ++i) {
                uint32_t tok = src[i];
                *dst_tok++ = tok;
                *dst_pos++ = static_cast<uint32_t>(i);
                h = mix(h, tok);
                h = mix(h, static_cast<uint32_t>(i));
            }
            src += max_len; // advance to next row
        }
        hash = h;
    }
    return hash;
}