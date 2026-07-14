#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Pre-compute total packed size
  size_t total_packed = 0;
  for (int b = 0; b < batch; ++b) {
    total_packed += static_cast<size_t>(lengths[static_cast<size_t>(b)]);
  }
  
  // Pre-allocate buffers once
  std::vector<uint32_t> packed_tokens(total_packed);
  std::vector<uint32_t> positions(total_packed);
  
  // Pre-compute prefix sums for output offsets
  std::vector<size_t> out_offsets(static_cast<size_t>(batch) + 1);
  out_offsets[0] = 0;
  for (int b = 0; b < batch; ++b) {
    out_offsets[static_cast<size_t>(b) + 1] = out_offsets[static_cast<size_t>(b)] + static_cast<size_t>(lengths[static_cast<size_t>(b)]);
  }
  
  const uint32_t* __restrict src = padded_tokens.data();
  uint32_t* __restrict dst_tokens = packed_tokens.data();
  uint32_t* __restrict dst_pos = positions.data();
  const size_t ml = static_cast<size_t>(max_len);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Pack tokens using memcpy for each row (contiguous in source)
    for (int b = 0; b < batch; ++b) {
      const size_t len = static_cast<size_t>(lengths[static_cast<size_t>(b)]);
      const size_t src_offset = static_cast<size_t>(b) * ml;
      const size_t dst_offset = out_offsets[static_cast<size_t>(b)];
      
      std::memcpy(dst_tokens + dst_offset, src + src_offset, len * sizeof(uint32_t));
      
      // Fill positions 0, 1, 2, ..., len-1
      uint32_t* __restrict pos_ptr = dst_pos + dst_offset;
      size_t i = 0;
      // Unroll by 4 for better vectorization
      for (; i + 4 <= len; i += 4) {
        pos_ptr[i] = static_cast<uint32_t>(i);
        pos_ptr[i + 1] = static_cast<uint32_t>(i + 1);
        pos_ptr[i + 2] = static_cast<uint32_t>(i + 2);
        pos_ptr[i + 3] = static_cast<uint32_t>(i + 3);
      }
      for (; i < len; ++i) {
        pos_ptr[i] = static_cast<uint32_t>(i);
      }
    }
    
    // Compute hash - process tokens first, then positions
    // This matches the original interleaved order: token[0], pos[0], token[1], pos[1], ...
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < total_packed; ++i) {
      hash = mix(hash, dst_tokens[i]);
      hash = mix(hash, dst_pos[i]);
    }
  }
  
  return hash;
}