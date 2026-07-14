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

constexpr uint64_t kFnvBasis = 1469598103934665603ULL;

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  if (beams == 0 || topk == 0 || iters == 0) {
    return kFnvBasis;
  }

  const std::size_t num_beams = static_cast<std::size_t>(beams);
  const std::size_t vocab_size = static_cast<std::size_t>(vocab);
  const std::size_t k = static_cast<std::size_t>(topk);
  const std::size_t num_iters = static_cast<std::size_t>(iters);

  // Flat storage for all top-k pairs: [beam0_k0, beam0_k1, ..., beam1_k0, ...]
  std::vector<std::pair<int32_t, int>> all_topk(num_beams * k);

  // Reusable working vector for one beam
  std::vector<std::pair<int32_t, int>> row(vocab_size);

  for (std::size_t b = 0; b < num_beams; ++b) {
    const std::size_t base = b * vocab_size;
    for (std::size_t v = 0; v < vocab_size; ++v) {
      row[v].first = logits[base + v];
      row[v].second = static_cast<int>(v);
    }

    // Place the k largest (with tie‑break) at the front, sorted
    std::partial_sort(row.begin(), row.begin() + k, row.end(), better);

    // Copy the top‑k into the flat array
    std::copy_n(row.begin(), k, all_topk.begin() + b * k);
  }

  uint64_t hash = 0;
  for (std::size_t iter = 0; iter < num_iters; ++iter) {
    hash = kFnvBasis;
    for (std::size_t b = 0; b < num_beams; ++b) {
      const std::size_t beam_offset = b * k;
      for (std::size_t i = 0; i < k; ++i) {
        const auto& p = all_topk[beam_offset + i];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(p.first)));
        hash = mix(hash, static_cast<uint64_t>(p.second));
      }
    }
  }

  return hash;
}
