#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

#define FNV_OFFSET_BASIS 1469598103934665603ULL
#define FNV_PRIME 1099511628211ULL

static uint16_t g_blend_lut[256][256];
static int g_blend_lut_init = 0;

static void init_blend_lut(void) {
  if (g_blend_lut_init) return;
  for (uint32_t a = 0; a < 256u; ++a) {
    for (uint32_t x = 0; x < 256u; ++x) {
      g_blend_lut[a][x] = (uint16_t)((x * a + 127u) / 255u);
    }
  }
  g_blend_lut_init = 1;
}

static inline uint64_t fnv1a_u8(uint64_t h, uint8_t v) {
  h ^= (uint64_t)v;
  h *= FNV_PRIME;
  return h;
}

static inline uint8_t blend_lut(uint8_t s, uint8_t d, uint8_t a) {
  return (uint8_t)(g_blend_lut[a][s] + g_blend_lut[255u - a][d]);
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  init_blend_lut();

  size_t pixels = (size_t)width * (size_t)height;
  size_t total_bytes = pixels * 4u;
  uint64_t hash = 0;

  if (iters <= 0) return 0;

  uint8_t* out = (uint8_t*)malloc(total_bytes);
  if (!out) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET_BASIS;
    size_t i = 0;

    for (; i + 4u <= pixels; i += 4u) {
      size_t idx = i * 4u;

      for (unsigned k = 0; k < 4u; ++k) {
        size_t p = idx + ((size_t)k << 2);
        uint8_t a = src[p + 3u];
        uint8_t r, g, b;

        if (a == 0u) {
          r = dst[p];
          g = dst[p + 1u];
          b = dst[p + 2u];
        } else if (a == 255u) {
          r = src[p];
          g = src[p + 1u];
          b = src[p + 2u];
        } else {
          r = blend_lut(src[p], dst[p], a);
          g = blend_lut(src[p + 1u], dst[p + 1u], a);
          b = blend_lut(src[p + 2u], dst[p + 2u], a);
        }

        out[p] = r;
        out[p + 1u] = g;
        out[p + 2u] = b;
        out[p + 3u] = 255u;

        h = fnv1a_u8(h, r);
        h = fnv1a_u8(h, g);
        h = fnv1a_u8(h, b);
        h = fnv1a_u8(h, 255u);
      }
    }

    for (; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      uint8_t r, g, b;

      if (a == 0u) {
        r = dst[idx];
        g = dst[idx + 1u];
        b = dst[idx + 2u];
      } else if (a == 255u) {
        r = src[idx];
        g = src[idx + 1u];
        b = src[idx + 2u];
      } else {
        r = blend_lut(src[idx], dst[idx], a);
        g = blend_lut(src[idx + 1u], dst[idx + 1u], a);
        b = blend_lut(src[idx + 2u], dst[idx + 2u], a);
      }

      out[idx] = r;
      out[idx + 1u] = g;
      out[idx + 2u] = b;
      out[idx + 3u] = 255u;

      h = fnv1a_u8(h, r);
      h = fnv1a_u8(h, g);
      h = fnv1a_u8(h, b);
      h = fnv1a_u8(h, 255u);
    }

    hash = h;
  }

  free(out);
  return hash;
}
