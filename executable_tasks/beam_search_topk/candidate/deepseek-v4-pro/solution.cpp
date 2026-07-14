#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace {
bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  if (iters == 0) return 0;
  const int k = std::min(topk, vocab);
  // Precompute top-k per beam
  std::vector<std::pair<int32_t, int>> beam_tokens(static_cast<std::size_t>(beams) * static_cast<std::size_t>(k));
  std::vector<std::pair<int32_t, int>> row;
  row.reserve(vocab);
  for (int b = 0; b < beams; ++b) {
    row.clear();
    const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
    for (int v = 0; v < vocab; ++v) {
      row.push_back({logits[base + static_cast<std::size_t>(v)], v});
    }
    std::partial_sort(row.begin(), row.begin() + k, row.end(), better);
    // copy to beam_tokens
    std::copy(row.begin(), row.begin() + k, beam_tokens.begin() + static_cast<std::size_t>(b) * k);
  }
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const auto* beam_ptr = beam_tokens.data() + static_cast<std::size_t>(b) * k;
      for (int i = 0; i < k; ++i) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(beam_ptr[i].first)));
        hash = mix(hash, static_cast<uint64_t>(beam_ptr[i].second));
      }
    }
  }
  return hash;
}