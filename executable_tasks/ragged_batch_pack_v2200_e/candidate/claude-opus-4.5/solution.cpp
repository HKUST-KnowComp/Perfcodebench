#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

__attribute__((always_inline)) inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Pre-compute total packed length
  std::size_t total_len = 0;
  for (int b = 0; b < batch; ++b) {
    total_len += static_cast<std::size_t>(lengths[static_cast<std::size_t>(b)]);
  }
  
  // Pre-allocate vectors once
  std::vector<uint32_t> packed_tokens(total_len);
  std::vector<uint32_t> positions(total_len);
  
  // Pre-compute row offsets for faster access
  std::vector<std::size_t> row_offsets(static_cast<std::size_t>(batch));
  std::vector<std::size_t> pack_offsets(static_cast<std::size_t>(batch) + 1);
  pack_offsets[0] = 0;
  for (int b = 0; b < batch; ++b) {
    row_offsets[static_cast<std::size_t>(b)] = static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
    pack_offsets[static_cast<std::size_t>(b) + 1] = pack_offsets[static_cast<std::size_t>(b)] + static_cast<std::size_t>(lengths[static_cast<std::size_t>(b)]);
  }
  
  const uint32_t* __restrict__ padded_ptr = padded_tokens.data();
  uint32_t* __restrict__ packed_ptr = packed_tokens.data();
  uint32_t* __restrict__ pos_ptr = positions.data();
  const int* __restrict__ len_ptr = lengths.data();
  const std::size_t* __restrict__ row_off_ptr = row_offsets.data();
  const std::size_t* __restrict__ pack_off_ptr = pack_offsets.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Pack tokens and positions
    for (int b = 0; b < batch; ++b) {
      const int len = len_ptr[b];
      const std::size_t src_offset = row_off_ptr[b];
      std::size_t dst_offset = pack_off_ptr[b];
      
      // Prefetch next batch's source data
      if (b + 1 < batch) {
        __builtin_prefetch(padded_ptr + row_off_ptr[b + 1], 0, 1);
      }
      
      for (int i = 0; i < len; ++i) {
        packed_ptr[dst_offset] = padded_ptr[src_offset + static_cast<std::size_t>(i)];
        pos_ptr[dst_offset] = static_cast<uint32_t>(i);
        ++dst_offset;
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < total_len; ++i) {
      hash = mix(hash, packed_ptr[i]);
      hash = mix(hash, pos_ptr[i]);
    }
  }
  
  return hash;
}