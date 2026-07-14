#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define HAS_AVX2 1
#else
#define HAS_AVX2 0
#endif

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {

  const size_t num_probes = probes.size();
  const size_t num_boundaries = boundaries.size();

  if (num_probes == 0 || iters <= 0) {
    return 0;
  }

  const uint32_t* __restrict__ p_probes = probes.data();
  const uint32_t* __restrict__ p_bounds = boundaries.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t bi = 0;  // boundary index

#if HAS_AVX2
    __m256i vsum = _mm256_setzero_si256();
    size_t pi = 0;

    // Process 4 probes at a time with scalar boundary advancement
    // but SIMD accumulation
    const size_t simd_end = (num_probes >= 3) ? (num_probes - 3) : 0;

    while (pi < simd_end) {
      // Prefetch future probes
      if (pi + 64 < num_probes) {
        __builtin_prefetch(&p_probes[pi + 64], 0, 0);
      }

      uint32_t probe0 = p_probes[pi];
      uint32_t probe1 = p_probes[pi + 1];
      uint32_t probe2 = p_probes[pi + 2];
      uint32_t probe3 = p_probes[pi + 3];

      // Advance boundary for probe0
      while (bi < num_boundaries && p_bounds[bi] <= probe0) {
        ++bi;
      }
      uint64_t idx0 = bi;

      // Advance boundary for probe1 (probes are sorted, so bi only increases)
      while (bi < num_boundaries && p_bounds[bi] <= probe1) {
        ++bi;
      }
      uint64_t idx1 = bi;

      // Advance boundary for probe2
      while (bi < num_boundaries && p_bounds[bi] <= probe2) {
        ++bi;
      }
      uint64_t idx2 = bi;

      // Advance boundary for probe3
      while (bi < num_boundaries && p_bounds[bi] <= probe3) {
        ++bi;
      }
      uint64_t idx3 = bi;

      // Pack into 64-bit lanes and accumulate
      __m256i vidx = _mm256_set_epi64x(
          static_cast<int64_t>(idx3),
          static_cast<int64_t>(idx2),
          static_cast<int64_t>(idx1),
          static_cast<int64_t>(idx0));
      vsum = _mm256_add_epi64(vsum, vidx);

      pi += 4;
    }

    // Horizontal sum of vsum
    alignas(32) uint64_t temp[4];
    _mm256_store_si256(reinterpret_cast<__m256i*>(temp), vsum);
    sum = temp[0] + temp[1] + temp[2] + temp[3];

    // Handle remaining probes
    for (; pi < num_probes; ++pi) {
      uint32_t probe = p_probes[pi];
      while (bi < num_boundaries && p_bounds[bi] <= probe) {
        ++bi;
      }
      sum += bi;
    }

#else
    // Scalar fallback: linear merge
    for (size_t pi = 0; pi < num_probes; ++pi) {
      uint32_t probe = p_probes[pi];
      while (bi < num_boundaries && p_bounds[bi] <= probe) {
        ++bi;
      }
      sum += bi;
    }
#endif
  }

  return sum;
}
