#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  // Allocate once and reuse across iterations
  uint64_t* hist = (uint64_t*)malloc((size_t)buckets * sizeof(uint64_t));
  uint32_t* tags = (uint32_t*)calloc((size_t)buckets, sizeof(uint32_t));
  if ((buckets && (!hist || !tags)) || (!buckets && (!hist && !tags))) {
    // Allocation failed (handle buckets==0 gracefully)
    free(hist);
    free(tags);
    return 0;
  }

  const uint32_t* restrict kptr = keys;
  const uint32_t* restrict vptr = vals;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t hash = 0;
  uint32_t epoch = 1u;

  for (int iter = 0; iter < iters; ++iter) {
    if (epoch == 0u) {
      // Rare wrap-around protection
      if (buckets) memset(tags, 0, (size_t)buckets * sizeof(uint32_t));
      epoch = 1u;
    }

    // Accumulate into histogram using epoch-based lazy clearing
    size_t i = 0;
    size_t n4 = n & ~(size_t)3;
    for (; i < n4; i += 4) {
      uint32_t k0 = kptr[i + 0];
      uint32_t k1 = kptr[i + 1];
      uint32_t k2 = kptr[i + 2];
      uint32_t k3 = kptr[i + 3];

      uint32_t v0 = vptr[i + 0];
      uint32_t v1 = vptr[i + 1];
      uint32_t v2 = vptr[i + 2];
      uint32_t v3 = vptr[i + 3];

      if (tags[k0] != epoch) { tags[k0] = epoch; hist[k0] = (uint64_t)v0; } else { hist[k0] += (uint64_t)v0; }
      if (tags[k1] != epoch) { tags[k1] = epoch; hist[k1] = (uint64_t)v1; } else { hist[k1] += (uint64_t)v1; }
      if (tags[k2] != epoch) { tags[k2] = epoch; hist[k2] = (uint64_t)v2; } else { hist[k2] += (uint64_t)v2; }
      if (tags[k3] != epoch) { tags[k3] = epoch; hist[k3] = (uint64_t)v3; } else { hist[k3] += (uint64_t)v3; }
    }
    for (; i < n; ++i) {
      uint32_t k = kptr[i];
      uint64_t addv = (uint64_t)vptr[i];
      if (tags[k] != epoch) { tags[k] = epoch; hist[k] = addv; }
      else { hist[k] += addv; }
    }

    // Compute FNV-1a hash over all buckets, treating unseen buckets as zero
    hash = FNV_OFFSET;
    uint32_t b = 0;
    uint32_t b4 = buckets & ~3u;
    for (; b < b4; b += 4) {
      uint64_t x0 = ((tags[b + 0] == epoch) ? hist[b + 0] : 0ULL) + 1ULL;
      hash ^= x0; hash *= FNV_PRIME;
      uint64_t x1 = ((tags[b + 1] == epoch) ? hist[b + 1] : 0ULL) + 1ULL;
      hash ^= x1; hash *= FNV_PRIME;
      uint64_t x2 = ((tags[b + 2] == epoch) ? hist[b + 2] : 0ULL) + 1ULL;
      hash ^= x2; hash *= FNV_PRIME;
      uint64_t x3 = ((tags[b + 3] == epoch) ? hist[b + 3] : 0ULL) + 1ULL;
      hash ^= x3; hash *= FNV_PRIME;
    }
    for (; b < buckets; ++b) {
      uint64_t x = ((tags[b] == epoch) ? hist[b] : 0ULL) + 1ULL;
      hash ^= x;
      hash *= FNV_PRIME;
    }

    ++epoch;
  }

  free(hist);
  free(tags);
  return hash;
}
