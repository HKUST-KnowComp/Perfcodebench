#pragma once

#include <cstdint>
#include <vector>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  
  // Pre-calculate total tokens to avoid reallocations
  size_t total_tokens = 0;
  for (int b = 0; b < batch; ++b) {
    total_tokens += lengths[static_cast<std::size_t>(b)];
  }
  
  packed_tokens.reserve(total_tokens);
  positions.reserve(total_tokens);
  
  // Precompute packed data once
  for (int b = 0; b < batch; ++b) {
    int len = lengths[static_cast<std::size_t>(b)];
    for (int i = 0; i < len; ++i) {
      packed_tokens.push_back(
          padded_tokens[static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len) + static_cast<std::size_t>(i)]);
      positions.push_back(static_cast<uint32_t>(i));
    }
  }
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < packed_tokens.size(); ++i) {
      hash ^= packed_tokens[i];
      hash *= 1099511628211ULL;
      hash ^= positions[i];
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}