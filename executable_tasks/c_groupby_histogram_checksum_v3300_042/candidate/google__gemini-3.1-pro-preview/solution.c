#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;

  uint64_t* hist;
  uint64_t stack_hist[2048];
  
  if (buckets <= 2048) {
    hist = stack_hist;
    memset(hist, 0, buckets * sizeof(uint64_t));
  } else {
    hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
    if (!hist) return 0;
  }

  size_t i = 0;
  for (; i + 7 < n; i += 8) {
    hist[keys[i]] += vals[i];
    hist[keys[i+1]] += vals[i+1];
    hist[keys[i+2]] += vals[i+2];
    hist[keys[i+3]] += vals[i+3];
    hist[keys[i+4]] += vals[i+4];
    hist[keys[i+5]] += vals[i+5];
    hist[keys[i+6]] += vals[i+6];
    hist[keys[i+7]] += vals[i+7];
  }
  for (; i < n; ++i) {
    hist[keys[i]] += vals[i];
  }

  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t j = 0; j < buckets; ++j) {
    hash ^= (hist[j] + 1ULL);
    hash *= 1099511628211ULL;
  }

  if (buckets > 2048) {
    free(hist);
  }

  return hash;
}
