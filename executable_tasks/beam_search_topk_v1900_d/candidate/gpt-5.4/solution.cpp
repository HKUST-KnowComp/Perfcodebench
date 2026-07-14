#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct Item {
  int32_t score;
  int id;
};

static inline bool better_item(const Item& a, const Item& b) {
  return (a.score > b.score) || (a.score == b.score && a.id < b.id);
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

  std::vector<Item> buf(static_cast<std::size_t>(vocab));

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);

      for (int v = 0; v < vocab; ++v) {
        buf[static_cast<std::size_t>(v)] = Item{logits[base + static_cast<std::size_t>(v)], v};
      }

      Item* begin = buf.data();
      Item* mid = begin + topk;
      Item* end = begin + vocab;

      std::nth_element(begin, mid, end, [](const Item& a, const Item& b) {
        return better_item(a, b);
      });
      std::sort(begin, mid, [](const Item& a, const Item& b) {
        return better_item(a, b);
      });

      for (int k = 0; k < topk; ++k) {
        const Item& it = begin[k];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(it.score)));
        hash = mix(hash, static_cast<uint64_t>(it.id));
      }
    }
  }

  return hash;
}
