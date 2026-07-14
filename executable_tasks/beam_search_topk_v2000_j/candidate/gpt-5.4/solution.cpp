#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct Entry {
  int32_t score;
  int idx;
};

static inline bool better_entry(const Entry& a, const Entry& b) {
  return (a.score > b.score) || (a.score == b.score && a.idx < b.idx);
}

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  uint64_t hash = 0;
  if (beams <= 0 || vocab <= 0 || topk <= 0 || iters <= 0) {
    return hash;
  }

  if (topk > vocab) topk = vocab;

  std::vector<Entry> best;
  best.resize(static_cast<std::size_t>(topk));

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);

      int filled = 0;
      for (; filled < topk; ++filled) {
        best[static_cast<std::size_t>(filled)] = Entry{logits[base + static_cast<std::size_t>(filled)], filled};
      }

      std::sort(best.begin(), best.end(), better_entry);

      for (int v = topk; v < vocab; ++v) {
        const Entry cand{logits[base + static_cast<std::size_t>(v)], v};
        if (!better_entry(cand, best[static_cast<std::size_t>(topk - 1)])) {
          continue;
        }

        int pos = topk - 1;
        while (pos > 0 && better_entry(cand, best[static_cast<std::size_t>(pos - 1)])) {
          best[static_cast<std::size_t>(pos)] = best[static_cast<std::size_t>(pos - 1)];
          --pos;
        }
        best[static_cast<std::size_t>(pos)] = cand;
      }

      for (int k = 0; k < topk; ++k) {
        const Entry& e = best[static_cast<std::size_t>(k)];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(e.score)));
        hash = mix(hash, static_cast<uint64_t>(e.idx));
      }
    }
  }

  return hash;
}
