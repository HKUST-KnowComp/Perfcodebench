#include "interface.h"

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  // Baseline returns 0 if iters <= 0
  if (iters <= 0) return 0ULL;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET;

  const std::size_t n_rows = row_ptr.size() > 0 ? (row_ptr.size() - 1) : 0;
  const uint32_t* rp = row_ptr.data();
  const uint32_t* vals = values.data();

  for (std::size_t r = 0; r < n_rows; ++r) {
    std::size_t start = static_cast<std::size_t>(rp[r]);
    std::size_t end   = static_cast<std::size_t>(rp[r + 1]);

    const uint32_t* p = vals + start;
    const uint32_t* q = vals + end;

    uint64_t sum = 0;

    // Unrolled accumulation for better throughput
    for (; p + 8 <= q; p += 8) {
      sum += p[0];
      sum += p[1];
      sum += p[2];
      sum += p[3];
      sum += p[4];
      sum += p[5];
      sum += p[6];
      sum += p[7];
    }
    for (; p < q; ++p) {
      sum += *p;
    }

    // FNV-1a style mixing per row
    hash ^= sum;
    hash *= FNV_PRIME;
  }

  return hash;
}
