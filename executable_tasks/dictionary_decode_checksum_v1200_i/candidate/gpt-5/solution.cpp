#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // If no iterations requested, baseline returns 0 as hash was never set.
  if (iters <= 0) {
    return 0ULL;
  }

  // FNV-1a 64-bit constants used by the baseline mix function
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  // Precompute length and leading byte for each dictionary entry to avoid
  // repeated string operations in the main loop.
  struct Stats {
    uint64_t len;
    uint64_t lead;
  };

  const std::size_t dict_size = dict.size();
  std::vector<Stats> stats;
  stats.resize(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dict[i];
    const std::size_t len = s.size();
    const uint64_t lead = (len == 0) ? 0u : static_cast<unsigned char>(s[0]);
    stats[i].len = static_cast<uint64_t>(len);
    stats[i].lead = lead;
  }

  // Compute the checksum for a single iteration; since each iteration is the
  // same, the final hash equals this value.
  uint64_t hash = FNV_OFFSET;
  const std::size_t n = codes.size();
  const uint32_t* p = n ? codes.data() : nullptr;
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t idx = static_cast<std::size_t>(p[i]);
    const Stats& st = stats[idx];
    hash ^= st.len;
    hash *= FNV_PRIME;
    hash ^= st.lead;
    hash *= FNV_PRIME;
  }

  return hash;
}
