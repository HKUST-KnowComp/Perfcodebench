#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const uint32_t* bdata = boundaries.data();
  const size_t nb = boundaries.size();
  const uint32_t* pdata = probes.data();
  const size_t np = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* bptr = bdata;
    const uint32_t* const bend = bdata + nb;
    uint64_t s = 0;

    const uint32_t* pptr = pdata;
    const uint32_t* const pend = pdata + np;

    for (; pptr != pend; ++pptr) {
      const uint32_t v = *pptr;
      while (bptr != bend && *bptr <= v) {
        ++bptr;
      }
      s += static_cast<uint64_t>(bptr - bdata);
    }

    sum = s;  // return result of the last iteration, matching baseline behavior
  }

  return sum;
}
