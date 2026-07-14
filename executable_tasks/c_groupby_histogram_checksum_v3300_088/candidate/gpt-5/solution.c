#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;

  uint64_t* hist = NULL;
  uint32_t* gen = NULL;

  if (buckets > 0) {
    hist = (uint64_t*)malloc((size_t)buckets * sizeof(uint64_t));
    gen  = (uint32_t*)malloc((size_t)buckets * sizeof(uint32_t));
    if (!hist || !gen) {
      free(hist);
      free(gen);
      return 0;
    }
    for (uint32_t i = 0; i < buckets; ++i) gen[i] = 0;
  }

  uint32_t cur = 1; // current generation marker (0 means "not current")

  for (int iter = 0; iter < iters; ++iter) {
    // accumulate values into histogram for this iteration
    if (buckets > 0) {
      const uint32_t* k = keys;
      const uint32_t* v = vals;
      for (size_t i = 0; i < n; ++i) {
        uint32_t idx = k[i];
        // assume idx < buckets per problem contract
        if (gen[idx] != cur) { // first touch in this iteration
          hist[idx] = 0;
          gen[idx] = cur;
        }
        hist[idx] += (uint64_t)v[i];
      }
    }

    // compute checksum over all buckets
    uint64_t h = 1469598103934665603ULL; // FNV offset basis
    if (buckets > 0) {
      for (uint32_t i = 0; i < buckets; ++i) {
        uint64_t v = (gen[i] == cur) ? hist[i] : 0ULL;
        h ^= (v + 1ULL);
        h *= 1099511628211ULL; // FNV prime
      }
    }
    hash = h;

    // advance generation, handle wrap
    cur += 1;
    if (cur == 0) { // wrapped to 0; reset markers to avoid false matches
      if (buckets > 0) {
        for (uint32_t i = 0; i < buckets; ++i) gen[i] = 0;
      }
      cur = 1;
    }
  }

  free(hist);
  free(gen);
  return hash;
}
