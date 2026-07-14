#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;

  const size_t nb = boundaries.size();
  const size_t np = probes.size();
  if (np == 0 || nb == 0) {
    return 0; // no probes or no boundaries => sum is zero regardless of iters
  }

  const uint32_t* b = boundaries.data();
  const uint32_t* p = probes.data();

  size_t j = 0; // count of boundaries <= current probe
  uint64_t sum_once = 0;

  for (size_t i = 0; i < np; ++i) {
    const uint32_t pv = p[i];
    while (j < nb && b[j] <= pv) {
      ++j;
    }
    sum_once += static_cast<uint64_t>(j);
  }

  unsigned __int128 total = static_cast<unsigned __int128>(sum_once) * static_cast<unsigned __int128>(static_cast<uint64_t>(iters));
  return static_cast<uint64_t>(total);
}
