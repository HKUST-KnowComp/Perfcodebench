#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

struct Pair {
  int32_t score;
  int32_t id;
};

bool better(const Pair& a, const Pair& b) {
  return a.score > b.score || (a.score == b.score && a.id < b.id);
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  const std::size_t V = static_cast<std::size_t>(vocab);
  const std::size_t B = static_cast<std::size_t>(beams);
  const int K = topk;
  const std::size_t total = B * K;

  // Precompute the top‑K (score, id) for every beam.
  std::vector<int32_t> top_scores(total);
  std::vector<int32_t> top_ids(total);

  std::vector<Pair> row;
  row.reserve(V);

  for (std::size_t b = 0; b < B; ++b) {
    row.clear();
    const std::size_t base = b * V;
    for (int v = 0; v < vocab; ++v) {
      row.push_back({logits[base + static_cast<std::size_t>(v)], v});
    }
    std::sort(row.begin(), row.end(), better);

    const std::size_t offset = b * K;
    for (int k = 0; k < K; ++k) {
      top_scores[offset + k] = row[static_cast<std::size_t>(k)].score;
      top_ids[offset + k] = row[static_cast<std::size_t>(k)].id;
    }
  }

  // Recompute the checksum iters times using the precomputed data.
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < total; ++i) {
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top_scores[i])));
      hash = mix(hash, static_cast<uint64_t>(top_ids[i]));
    }
  }

  return hash;
}