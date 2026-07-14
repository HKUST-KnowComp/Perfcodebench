#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  (void)num_pages;
  
  const uint32_t* __restrict__ cache_ptr = cache.data();
  const uint32_t* __restrict__ pids = page_ids.data();
  const uint32_t* __restrict__ soffs = slot_offsets.data();
  const std::size_t n = page_ids.size();
  const uint64_t ps = static_cast<uint64_t>(page_size);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
#if defined(__AVX2__)
    // Process 8 elements at a time using AVX2 gather
    const __m256i vps = _mm256_set1_epi32(page_size);
    std::size_t i = 0;
    
    // Unroll by processing 8 at a time
    for (; i + 8 <= n; i += 8) {
      // Load page_ids and slot_offsets
      __m256i vpids = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(pids + i));
      __m256i vsoffs = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(soffs + i));
      
      // Compute indices: page_id * page_size + slot_offset
      __m256i vidx = _mm256_add_epi32(_mm256_mullo_epi32(vpids, vps), vsoffs);
      
      // Gather values from cache
      __m256i vvals = _mm256_i32gather_epi32(reinterpret_cast<const int*>(cache_ptr), vidx, 4);
      
      // Extract and hash sequentially (must maintain order for correctness)
      alignas(32) uint32_t vals[8];
      _mm256_store_si256(reinterpret_cast<__m256i*>(vals), vvals);
      
      hash = mix(hash, vals[0]);
      hash = mix(hash, vals[1]);
      hash = mix(hash, vals[2]);
      hash = mix(hash, vals[3]);
      hash = mix(hash, vals[4]);
      hash = mix(hash, vals[5]);
      hash = mix(hash, vals[6]);
      hash = mix(hash, vals[7]);
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint64_t idx = static_cast<uint64_t>(pids[i]) * ps + static_cast<uint64_t>(soffs[i]);
      hash = mix(hash, cache_ptr[idx]);
    }
#else
    // Scalar fallback with manual unrolling
    std::size_t i = 0;
    for (; i + 4 <= n; i += 4) {
      uint64_t idx0 = static_cast<uint64_t>(pids[i]) * ps + static_cast<uint64_t>(soffs[i]);
      uint64_t idx1 = static_cast<uint64_t>(pids[i+1]) * ps + static_cast<uint64_t>(soffs[i+1]);
      uint64_t idx2 = static_cast<uint64_t>(pids[i+2]) * ps + static_cast<uint64_t>(soffs[i+2]);
      uint64_t idx3 = static_cast<uint64_t>(pids[i+3]) * ps + static_cast<uint64_t>(soffs[i+3]);
      
      uint32_t v0 = cache_ptr[idx0];
      uint32_t v1 = cache_ptr[idx1];
      uint32_t v2 = cache_ptr[idx2];
      uint32_t v3 = cache_ptr[idx3];
      
      hash = mix(hash, v0);
      hash = mix(hash, v1);
      hash = mix(hash, v2);
      hash = mix(hash, v3);
    }
    
    for (; i < n; ++i) {
      uint64_t idx = static_cast<uint64_t>(pids[i]) * ps + static_cast<uint64_t>(soffs[i]);
      hash = mix(hash, cache_ptr[idx]);
    }
#endif
  }
  
  return hash;
}