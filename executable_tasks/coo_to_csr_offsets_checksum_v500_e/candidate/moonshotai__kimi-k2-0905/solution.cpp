#include "interface.h"
#include <immintrin.h>
#include <vector>
#include <cstdint>
#include <cstring>

static uint64_t mix2(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    const size_t nnz = row_ids.size();
    const size_t R = rows;
    const size_t R1 = R + 1;
    
    // reusable working memory
    static thread_local std::vector<uint32_t> tls_offsets;
    tls_offsets.clear();
    tls_offsets.resize((R + 1 + 8) & ~size_t(7), 0); // pad to multiple of 8

    uint32_t* off = tls_offsets.data();
    
    uint64_t hash = 1469598103934665603ULL;
    for (int it = 0; it < iters; ++it) {
        // zero the offset vector (fast memset)
        std::memset(off, 0, R1 * sizeof(uint32_t));

        // histogram: O(nnz)
        for (uint32_t id : row_ids) off[id]++;

        // prefix-sum in place for offsets[0..R] -> off[r+1] is row start/rptr
        uint32_t sum = 0;
        for (size_t r = 0; r < R; ++r) {
            uint32_t c = off[r];
            off[r] = sum;
            sum += c;
        }
        off[R] = sum; // final row ends

        // row start becomes size; compute sizes off[r] = off[r+1]-off[r]
        uint32_t prev = off[0];
        for (size_t r = 1; r <= R; ++r) {
            uint32_t nxt = off[r];
            off[r-1] = nxt - prev;
            prev = nxt;
        }
        off[R] = 0; // sentinel

        // SIMD hashing – 256 bit chunks (4×uint64)
        static constexpr uint64_t init = 1469598103934665603ULL;
        const uint64_t* p = reinterpret_cast<const uint64_t*>(off);
        size_t N8 = (R1*sizeof(uint32_t))/sizeof(uint64_t);
        __m256i h = _mm256_set1_epi64x(init);
        __m256i mult = _mm256_set1_epi64x(1099511628211ULL);
        size_t i = 0;
        for (; i + 3 <= N8; i += 4) {
            __m256i v = _mm256_loadu_si256((__m256i const*)(p + i));
            h = _mm256_xor_si256(h, v);
            h = _mm256_mul_epu32(h, mult); // low 32×32 part; OK as we mix full hash below
        }
        // reduce SIMD lanes
        uint64_t hbuf[4];
        _mm256_storeu_si256((__m256i*)hbuf, h);
        hash = init;
        for (int k = 0; k < 4; ++k) hash = mix2(hash, hbuf[k]);
        // scalar tail
        for (; i < N8; ++i) hash = mix2(hash, p[i]);

        // clear off[r] for next iter
        std::memset(off, 0, R1 * sizeof(uint32_t));
    }
    return hash;
}