#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  size_t i = 0;
  
  // Process 8 bytes at a time for better throughput
  for (; i + 7 < n; i += 8) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[i + 1];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[i + 2];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[i + 3];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[i + 4];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[i + 5];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[i + 6];
    hash *= 1099511628211ULL;
    hash ^= (uint64_t)data[i + 7];
    hash *= 1099511628211ULL;
  }
  
  // Handle remaining bytes
  for (; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t bytes = pixels * 4u;
  
  // Allocate once outside the loop
  uint8_t* out = (uint8_t*)malloc(bytes);
  if (!out) return 0;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    
    // Process 4 pixels at a time (16 bytes)
    for (; i + 3 < pixels; i += 4) {
      size_t idx0 = i * 4u;
      size_t idx1 = idx0 + 4u;
      size_t idx2 = idx0 + 8u;
      size_t idx3 = idx0 + 12u;
      
      // Pixel 0
      uint32_t a0 = src[idx0 + 3u];
      uint32_t inv_a0 = 255u - a0;
      out[idx0] = (uint8_t)(((uint32_t)src[idx0] * a0 + (uint32_t)dst[idx0] * inv_a0 + 127u) / 255u);
      out[idx0 + 1u] = (uint8_t)(((uint32_t)src[idx0 + 1u] * a0 + (uint32_t)dst[idx0 + 1u] * inv_a0 + 127u) / 255u);
      out[idx0 + 2u] = (uint8_t)(((uint32_t)src[idx0 + 2u] * a0 + (uint32_t)dst[idx0 + 2u] * inv_a0 + 127u) / 255u);
      out[idx0 + 3u] = 255u;
      
      // Pixel 1
      uint32_t a1 = src[idx1 + 3u];
      uint32_t inv_a1 = 255u - a1;
      out[idx1] = (uint8_t)(((uint32_t)src[idx1] * a1 + (uint32_t)dst[idx1] * inv_a1 + 127u) / 255u);
      out[idx1 + 1u] = (uint8_t)(((uint32_t)src[idx1 + 1u] * a1 + (uint32_t)dst[idx1 + 1u] * inv_a1 + 127u) / 255u);
      out[idx1 + 2u] = (uint8_t)(((uint32_t)src[idx1 + 2u] * a1 + (uint32_t)dst[idx1 + 2u] * inv_a1 + 127u) / 255u);
      out[idx1 + 3u] = 255u;
      
      // Pixel 2
      uint32_t a2 = src[idx2 + 3u];
      uint32_t inv_a2 = 255u - a2;
      out[idx2] = (uint8_t)(((uint32_t)src[idx2] * a2 + (uint32_t)dst[idx2] * inv_a2 + 127u) / 255u);
      out[idx2 + 1u] = (uint8_t)(((uint32_t)src[idx2 + 1u] * a2 + (uint32_t)dst[idx2 + 1u] * inv_a2 + 127u) / 255u);
      out[idx2 + 2u] = (uint8_t)(((uint32_t)src[idx2 + 2u] * a2 + (uint32_t)dst[idx2 + 2u] * inv_a2 + 127u) / 255u);
      out[idx2 + 3u] = 255u;
      
      // Pixel 3
      uint32_t a3 = src[idx3 + 3u];
      uint32_t inv_a3 = 255u - a3;
      out[idx3] = (uint8_t)(((uint32_t)src[idx3] * a3 + (uint32_t)dst[idx3] * inv_a3 + 127u) / 255u);
      out[idx3 + 1u] = (uint8_t)(((uint32_t)src[idx3 + 1u] * a3 + (uint32_t)dst[idx3 + 1u] * inv_a3 + 127u) / 255u);
      out[idx3 + 2u] = (uint8_t)(((uint32_t)src[idx3 + 2u] * a3 + (uint32_t)dst[idx3 + 2u] * inv_a3 + 127u) / 255u);
      out[idx3 + 3u] = 255u;
    }
    
    // Handle remaining pixels
    for (; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint32_t a = src[idx + 3u];
      uint32_t inv_a = 255u - a;
      out[idx] = (uint8_t)(((uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a + 127u) / 255u);
      out[idx + 1u] = (uint8_t)(((uint32_t)src[idx + 1u] * a + (uint32_t)dst[idx + 1u] * inv_a + 127u) / 255u);
      out[idx + 2u] = (uint8_t)(((uint32_t)src[idx + 2u] * a + (uint32_t)dst[idx + 2u] * inv_a + 127u) / 255u);
      out[idx + 3u] = 255u;
    }
    
    hash = checksum_bytes(out, bytes);
  }
  
  free(out);
  return hash;
}