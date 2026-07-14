#include "interface.h"

#include <vector>
#include <cstddef>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Pre-compute total packed length
  std::size_t total_len = 0;
  for (int b = 0; b < batch; ++b) {
    total_len += static_cast<std::size_t>(lengths[static_cast<std::size_t>(b)]);
  }

  // Pre-allocate vectors once
  std::vector<uint32_t> packed_tokens(total_len);
  std::vector<uint32_t> positions(total_len);

  // Pre-compute row offsets for padded_tokens
  std::vector<std::size_t> row_offsets(static_cast<std::size_t>(batch));
  for (int b = 0; b < batch; ++b) {
    row_offsets[static_cast<std::size_t>(b)] = static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
  }

  const uint32_t* __restrict__ padded_ptr = padded_tokens.data();
  uint32_t* __restrict__ packed_ptr = packed_tokens.data();
  uint32_t* __restrict__ pos_ptr = positions.data();
  const int* __restrict__ len_ptr = lengths.data();
  const std::size_t* __restrict__ offset_ptr = row_offsets.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Pack tokens and positions
    std::size_t out_idx = 0;
    for (int b = 0; b < batch; ++b) {
      const int len = len_ptr[b];
      const std::size_t row_start = offset_ptr[b];
      const uint32_t* __restrict__ row_ptr = padded_ptr + row_start;
      
      for (int i = 0; i < len; ++i) {
        packed_ptr[out_idx] = row_ptr[i];
        pos_ptr[out_idx] = static_cast<uint32_t>(i);
        ++out_idx;
      }
    }

    // Compute hash
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < total_len; ++i) {
      hash ^= packed_ptr[i];
      hash *= 1099511628211ULL;
      hash ^= pos_ptr[i];
      hash *= 1099511628211ULL;
    }
  }

  return hash;
}