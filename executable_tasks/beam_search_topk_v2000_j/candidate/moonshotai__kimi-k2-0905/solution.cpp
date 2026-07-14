#include "interface.h"
#include <algorithm>
#include <limits>
#include <vector>

namespace {

struct KV {
  int32_t v;   // logit
  int32_t id;  // vocab index
  KV() = default;
  constexpr KV(int32_t logit, int32_t i) : v(logit), id(i) {}
};

inline bool operator<(const KV& a, const KV& b) noexcept {
  // Deterministic tie-break by index (ascending)
  return a.v > b.v || (a.v == b.v && a.id < b.id);
}

// Mix identical to baseline
inline uint64_t mix(uint64_t h, uint64_t x) {
  h ^= x;
  h *= 1099511628211ULL;
  return h;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                              int beams, int vocab, int topk, int iters) {
  constexpr uint64_t FNV_INIT = 1469598103934665603ULL;
  uint64_t hash = FNV_INIT;

  const size_t vocabU = static_cast<size_t>(vocab);
  const size_t topkU  = static_cast<size_t>(topk);

  // reusable buffer (one per beam); small enough to keep hot in L1
  std::vector<KV> vec(vocabU);
  std::vector<KV> top(tmp);

  for (int it = 0; it < iters; ++it) {
    for (int b = 0; b < beams; ++b) {
      size_t base = static_cast<size_t>(b) * vocabU;

      // fill vec
      for (size_t j = 0; j < vocabU; ++j)
        vec[j] = KV{logits[base + j], static_cast<int32_t>(j)};

      // partial sort only topk elements, rest undefined but we don’t use them
      std::partial_sort(vec.begin(), vec.begin() + topkU, vec.end());

      // hash topk
      for (size_t k = 0; k < topkU; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(vec[k].v)));
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(vec[k].id)));
      }
    }
  }
  return hash;
}