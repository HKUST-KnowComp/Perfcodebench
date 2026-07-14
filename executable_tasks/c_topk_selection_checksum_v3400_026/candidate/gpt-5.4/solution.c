#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static int pair_better_vals(uint32_t ascore, uint32_t aindex, uint32_t bscore, uint32_t bindex) {
  if (ascore > bscore) return 1;
  if (ascore < bscore) return 0;
  return aindex < bindex;
}

static int pair_better(const Pair* a, const Pair* b) {
  if (a->score > b->score) return 1;
  if (a->score < b->score) return 0;
  return a->index < b->index;
}

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*)lhs;
  const Pair* b = (const Pair*)rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (iters <= 0) return 0;
  if (k <= 0 || n == 0) {
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
      h = 1469598103934665603ULL;
    }
    return h;
  }

  size_t kk = (size_t)k;
  if (kk > n) kk = n;

  Pair* top = (Pair*)malloc(kk * sizeof(Pair));
  if (!top) return 0;

  uint64_t h = 0;

  for (int iter = 0; iter < iters; ++iter) {
    size_t used = 0;

    for (size_t i = 0; i < n; ++i) {
      uint32_t s = scores[i];
      uint32_t idx = (uint32_t)i;

      if (used < kk) {
        size_t pos = used;
        while (pos > 0 && pair_better_vals(s, idx, top[pos - 1].score, top[pos - 1].index)) {
          top[pos] = top[pos - 1];
          --pos;
        }
        top[pos].score = s;
        top[pos].index = idx;
        ++used;
      } else {
        const Pair* worst = &top[kk - 1];
        if (!pair_better_vals(s, idx, worst->score, worst->index)) continue;

        size_t pos = kk - 1;
        while (pos > 0 && pair_better_vals(s, idx, top[pos - 1].score, top[pos - 1].index)) {
          top[pos] = top[pos - 1];
          --pos;
        }
        top[pos].score = s;
        top[pos].index = idx;
      }
    }

    if (kk > 1) {
      qsort(top, kk, sizeof(Pair), cmp_pair_desc);
    }

    h = 1469598103934665603ULL;
    for (size_t i = 0; i < kk; ++i) {
      h ^= ((uint64_t)top[i].score << 32) ^ top[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(top);
  return h;
}
