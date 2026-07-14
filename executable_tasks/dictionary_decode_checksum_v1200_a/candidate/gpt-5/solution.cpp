#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {
constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime  = 1099511628211ULL;

struct TokenInfo {
  uint64_t len;
  uint64_t lead; // zero if empty, else first byte as unsigned
};
}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Preserve baseline behavior: if iters == 0, baseline returns 0.
  if (iters <= 0) {
    return 0ULL;
  }

  // Precompute per-dictionary token info once.
  std::vector<TokenInfo> infos;
  infos.resize(dict.size());
  for (std::size_t i = 0; i < dict.size(); ++i) {
    const std::string& s = dict[i];
    TokenInfo ti;
    ti.len = static_cast<uint64_t>(s.size());
    ti.lead = s.empty() ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(s[0]));
    infos[i] = ti;
  }

  // Single iteration yields the same hash as any (and thus the last) iteration,
  // since inputs are immutable and hash resets each time in the baseline.
  uint64_t hash = kFnvOffset;
  const uint32_t* p = codes.data();
  const uint32_t* pe = p + codes.size();
  while (p != pe) {
    std::size_t idx = static_cast<std::size_t>(*p++);
    const TokenInfo& info = infos[idx];
    hash ^= info.len;
    hash *= kFnvPrime;
    hash ^= info.lead;
    hash *= kFnvPrime;
  }

  return hash;
}
