#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t degree_checksum(
    const std::vector<uint32_t>& src,
    const std::vector<uint32_t>& dst,
    uint32_t vertices,
    int iters) {
  (void)dst;
  std::vector<uint32_t> degrees(vertices);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(degrees.begin(), degrees.end(), 0);
    for (uint32_t v : src) {
      degrees[v]++;
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t degree : degrees) {
      hash ^= degree;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}