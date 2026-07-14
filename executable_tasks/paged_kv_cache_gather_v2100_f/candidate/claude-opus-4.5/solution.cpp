#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
    
    (void)num_pages;
    
    const uint32_t* __restrict cache_ptr = cache.data();
    const uint32_t* __restrict pids = page_ids.data();
    const uint32_t* __restrict soffs = slot_offsets.data();
    const std::size_t n = page_ids.size();
    const uint32_t ps = static_cast<uint32_t>(page_size);
    
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        
#if defined(__AVX2__)
        const __m256i vps = _mm256_set1_epi32(static_cast<int>(ps));
        std::size_t i = 0;
        
        // Process 8 elements at a time using AVX2 gather
        for (; i + 8 <= n; i += 8) {
            // Load page_ids and slot_offsets
            __m256i vpids = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(pids + i));
            __m256i vsoffs = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(soffs + i));
            
            // Compute indices: page_id * page_size + slot_offset
            __m256i vidx = _mm256_add_epi32(_mm256_mullo_epi32(vpids, vps), vsoffs);
            
            // Gather values from cache
            __m256i vvals = _mm256_i32gather_epi32(reinterpret_cast<const int*>(cache_ptr), vidx, 4);
            
            // Extract and hash sequentially to preserve order
            alignas(32) uint32_t vals[8];
            _mm256_store_si256(reinterpret_cast<__m256i*>(vals), vvals);
            
            hash ^= vals[0]; hash *= FNV_PRIME;
            hash ^= vals[1]; hash *= FNV_PRIME;
            hash ^= vals[2]; hash *= FNV_PRIME;
            hash ^= vals[3]; hash *= FNV_PRIME;
            hash ^= vals[4]; hash *= FNV_PRIME;
            hash ^= vals[5]; hash *= FNV_PRIME;
            hash ^= vals[6]; hash *= FNV_PRIME;
            hash ^= vals[7]; hash *= FNV_PRIME;
        }
        
        // Handle remaining elements
        for (; i < n; ++i) {
            uint32_t idx = pids[i] * ps + soffs[i];
            uint32_t val = cache_ptr[idx];
            hash ^= val;
            hash *= FNV_PRIME;
        }
#else
        // Scalar fallback
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t idx = pids[i] * ps + soffs[i];
            uint32_t val = cache_ptr[idx];
            hash ^= val;
            hash *= FNV_PRIME;
        }
#endif
    }
    
    return hash;
}
