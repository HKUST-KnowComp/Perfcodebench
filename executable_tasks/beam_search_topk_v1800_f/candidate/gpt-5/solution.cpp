#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct BetterIdx {
  const int32_t* row;
  inline bool operator()(int a, int b) const noexcept {
    const int32_t va = row[a];
    const int32_t vb = row[b];
    return (va > vb) || (va == vb && a < b);
  }
};

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  const std::size_t s_beams = static_cast<std::size_t>(beams);
  const std::size_t s_vocab = static_cast<std::size_t>(vocab);
  const std::size_t row_stride = s_vocab;

  uint64_t hash = 0;

  // Preallocate index buffer once; reused per beam
  std::vector<int> idx;
  idx.resize(s_vocab);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL; // FNV-1a offset basis

    if (topk == 1) {
      // Fast path for top-1: single pass argmax with deterministic tie-breaking
      for (std::size_t b = 0; b < s_beams; ++b) {
        const int32_t* row = logits.data() + b * row_stride;
        int best_i = 0;
        int32_t best_v = row[0];
        for (std::size_t v = 1; v < s_vocab; ++v) {
          const int32_t val = row[v];
          if (val > best_v) {
            best_v = val;
            best_i = static_cast<int>(v);
          }
          // On ties (val == best_v), smaller index wins; since v increases, do nothing
        }
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_v)));
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_i)));
      }
      continue;
    }

    const int use_topk = topk; // assume valid input (topk <= vocab)

    for (std::size_t b = 0; b < s_beams; ++b) {
      // Initialize indices 0..vocab-1
      for (std::size_t v = 0; v < s_vocab; ++v) idx[v] = static_cast<int>(v);

      const int32_t* row = logits.data() + b * row_stride;
      BetterIdx comp{row};

      // Place the top-k best (by comp) at the beginning in sorted order
      std::partial_sort(idx.begin(), idx.begin() + use_topk, idx.end(), comp);

      for (int k = 0; k < use_topk; ++k) {
        const int id = idx[static_cast<std::size_t>(k)];
        const int32_t score = row[id];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(score)));
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(id)));
      }
    }
  }

  return hash;
}
