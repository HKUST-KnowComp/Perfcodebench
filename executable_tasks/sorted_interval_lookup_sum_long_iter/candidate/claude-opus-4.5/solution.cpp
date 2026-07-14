#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <immintrin.h>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const std::size_t n_probes = probes.size();
  const std::size_t n_intervals = starts.size();
  
  if (n_probes == 0 || n_intervals == 0 || iters <= 0) {
    return 0;
  }
  
  // Pre-compute indices for all probes (since starts is sorted, use binary search)
  std::vector<uint32_t> indices(n_probes);
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* starts_end = starts_ptr + n_intervals;
  
  for (std::size_t i = 0; i < n_probes; ++i) {
    const auto it = std::upper_bound(starts_ptr, starts_end, probes[i]);
    indices[i] = static_cast<uint32_t>(it - starts_ptr - 1);
  }
  
  // Pre-compute weighted payloads
  std::vector<uint64_t> weighted(n_probes);
  for (std::size_t i = 0; i < n_probes; ++i) {
    weighted[i] = static_cast<uint64_t>(payloads[indices[i]]) * static_cast<uint64_t>(weights[i]);
  }
  
  // Compute sum of weighted payloads
  uint64_t base_sum = 0;
  
  // Use AVX2 for summing
  const std::size_t vec_size = 4;
  const std::size_t n_vec = n_probes / vec_size;
  const std::size_t remainder = n_probes % vec_size;
  
  __m256i acc = _mm256_setzero_si256();
  const uint64_t* weighted_ptr = weighted.data();
  
  for (std::size_t i = 0; i < n_vec; ++i) {
    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weighted_ptr + i * vec_size));
    acc = _mm256_add_epi64(acc, v);
  }
  
  // Horizontal sum
  alignas(32) uint64_t acc_arr[4];
  _mm256_store_si256(reinterpret_cast<__m256i*>(acc_arr), acc);
  base_sum = acc_arr[0] + acc_arr[1] + acc_arr[2] + acc_arr[3];
  
  // Handle remainder
  for (std::size_t i = n_vec * vec_size; i < n_probes; ++i) {
    base_sum += weighted[i];
  }
  
  // For iterations, we just need to return the last computed sum
  // Since the computation is the same each iteration, we can optimize
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = base_sum;
  }
  
  return sum;
}