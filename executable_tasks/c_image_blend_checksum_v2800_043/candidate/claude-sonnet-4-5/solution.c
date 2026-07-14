#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return (uint8_t)(((uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u) / 255u);
}

static uint64_t checksum_bytes(const uint8_t* restrict data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  size_t i = 0;
  
  // Process 8 bytes at a time
  for (; i + 7 < n; i += 8) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
    hash ^= (uint64_t)data[i + 1];
    hash *= prime;
    hash ^= (uint64_t)data[i + 2];
    hash *= prime;
    hash ^= (uint64_t)data[i + 3];
    hash *= prime;
    hash ^= (uint64_t)data[i + 4];
    hash *= prime;
    hash ^= (uint64_t)data[i + 5];
    hash *= prime;
    hash ^= (uint64_t)data[i + 6];
    hash *= prime;
    hash ^= (uint64_t)data[i + 7];
    hash *= prime;
  }
  
  // Handle remaining bytes
  for (; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
  }
  return hash;
}

uint64_t run(const uint8_t* restrict src, const uint8_t* restrict dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t bytes = pixels * 4u;
  uint64_t hash = 0;
  
  // Allocate once outside loop
  uint8_t* out = (uint8_t*)malloc(bytes);
  if (!out) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    
    // Process 4 pixels at a time (16 bytes)
    for (; i + 3 < pixels; i += 4) {
      size_t idx0 = i * 4u;
      size_t idx1 = idx0 + 4u;
      size_t idx2 = idx0 + 8u;
      size_t idx3 = idx0 + 12u;
      
      // Pixel 0
      uint8_t a0 = src[idx0 + 3u];
      out[idx0] = blend_byte(src[idx0], dst[idx0], a0);
      out[idx0 + 1u] = blend_byte(src[idx0 + 1u], dst[idx0 + 1u], a0);
      out[idx0 + 2u] = blend_byte(src[idx0 + 2u], dst[idx0 + 2u], a0);
      out[idx0 + 3u] = 255u;
      
      // Pixel 1
      uint8_t a1 = src[idx1 + 3u];
      out[idx1] = blend_byte(src[idx1], dst[idx1], a1);
      out[idx1 + 1u] = blend_byte(src[idx1 + 1u], dst[idx1 + 1u], a1);
      out[idx1 + 2u] = blend_byte(src[idx1 + 2u], dst[idx1 + 2u], a1);
      out[idx1 + 3u] = 255u;
      
      // Pixel 2
      uint8_t a2 = src[idx2 + 3u];
      out[idx2] = blend_byte(src[idx2], dst[idx2], a2);
      out[idx2 + 1u] = blend_byte(src[idx2 + 1u], dst[idx2 + 1u], a2);
      out[idx2 + 2u] = blend_byte(src[idx2 + 2u], dst[idx2 + 2u], a2);
      out[idx2 + 3u] = 255u;
      
      // Pixel 3
      uint8_t a3 = src[idx3 + 3u];
      out[idx3] = blend_byte(src[idx3], dst[idx3], a3);
      out[idx3 + 1u] = blend_byte(src[idx3 + 1u], dst[idx3 + 1u], a3);
      out[idx3 + 2u] = blend_byte(src[idx3 + 2u], dst[idx3 + 2u], a3);
      out[idx3 + 3u] = 255u;
    }
    
    // Handle remaining pixels
    for (; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      out[idx] = blend_byte(src[idx], dst[idx], a);
      out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a);
      out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a);
      out[idx + 3u] = 255u;
    }
    
    hash = checksum_bytes(out, bytes);
  }
  
  free(out);
  return hash;
}