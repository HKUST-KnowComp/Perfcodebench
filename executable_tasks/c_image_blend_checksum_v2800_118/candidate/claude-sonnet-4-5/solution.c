#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
  uint32_t tmp = (uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 128u;
  return (uint8_t)((tmp + (tmp >> 8)) >> 8);
}

static uint64_t checksum_bytes_fast(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  size_t i = 0;
  
  if (n >= 8) {
    uint64_t h0 = hash, h1 = hash, h2 = hash, h3 = hash;
    uint64_t h4 = hash, h5 = hash, h6 = hash, h7 = hash;
    
    for (; i + 7 < n; i += 8) {
      h0 ^= (uint64_t)data[i];
      h1 ^= (uint64_t)data[i + 1];
      h2 ^= (uint64_t)data[i + 2];
      h3 ^= (uint64_t)data[i + 3];
      h4 ^= (uint64_t)data[i + 4];
      h5 ^= (uint64_t)data[i + 5];
      h6 ^= (uint64_t)data[i + 6];
      h7 ^= (uint64_t)data[i + 7];
      
      h0 *= prime;
      h1 *= prime;
      h2 *= prime;
      h3 *= prime;
      h4 *= prime;
      h5 *= prime;
      h6 *= prime;
      h7 *= prime;
    }
    
    hash = h0 ^ h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h7;
  }
  
  for (; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
  }
  
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t bytes = pixels * 4u;
  uint64_t hash = 0;
  
  uint8_t* out = (uint8_t*)malloc(bytes);
  if (!out) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    
    for (; i + 3 < pixels; i += 4) {
      size_t idx0 = i * 4u;
      size_t idx1 = idx0 + 4u;
      size_t idx2 = idx0 + 8u;
      size_t idx3 = idx0 + 12u;
      
      uint8_t a0 = src[idx0 + 3u];
      uint8_t a1 = src[idx1 + 3u];
      uint8_t a2 = src[idx2 + 3u];
      uint8_t a3 = src[idx3 + 3u];
      
      out[idx0] = blend_byte_fast(src[idx0], dst[idx0], a0);
      out[idx0 + 1u] = blend_byte_fast(src[idx0 + 1u], dst[idx0 + 1u], a0);
      out[idx0 + 2u] = blend_byte_fast(src[idx0 + 2u], dst[idx0 + 2u], a0);
      out[idx0 + 3u] = 255u;
      
      out[idx1] = blend_byte_fast(src[idx1], dst[idx1], a1);
      out[idx1 + 1u] = blend_byte_fast(src[idx1 + 1u], dst[idx1 + 1u], a1);
      out[idx1 + 2u] = blend_byte_fast(src[idx1 + 2u], dst[idx1 + 2u], a1);
      out[idx1 + 3u] = 255u;
      
      out[idx2] = blend_byte_fast(src[idx2], dst[idx2], a2);
      out[idx2 + 1u] = blend_byte_fast(src[idx2 + 1u], dst[idx2 + 1u], a2);
      out[idx2 + 2u] = blend_byte_fast(src[idx2 + 2u], dst[idx2 + 2u], a2);
      out[idx2 + 3u] = 255u;
      
      out[idx3] = blend_byte_fast(src[idx3], dst[idx3], a3);
      out[idx3 + 1u] = blend_byte_fast(src[idx3 + 1u], dst[idx3 + 1u], a3);
      out[idx3 + 2u] = blend_byte_fast(src[idx3 + 2u], dst[idx3 + 2u], a3);
      out[idx3 + 3u] = 255u;
    }
    
    for (; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      out[idx] = blend_byte_fast(src[idx], dst[idx], a);
      out[idx + 1u] = blend_byte_fast(src[idx + 1u], dst[idx + 1u], a);
      out[idx + 2u] = blend_byte_fast(src[idx + 2u], dst[idx + 2u], a);
      out[idx + 3u] = 255u;
    }
    
    hash = checksum_bytes_fast(out, bytes);
  }
  
  free(out);
  return hash;
}
