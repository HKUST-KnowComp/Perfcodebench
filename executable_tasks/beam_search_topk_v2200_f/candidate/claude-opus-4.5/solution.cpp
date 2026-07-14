#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace {

inline bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
#ifdef _OPENMP
    std::vector<uint64_t> beam_hashes(static_cast<std::size_t>(beams));
    
    #pragma omp parallel
    {
      std::vector<std::pair<int32_t, int>> row;
      row.reserve(static_cast<std::size_t>(vocab));
      
      #pragma omp for schedule(static)
      for (int b = 0; b < beams; ++b) {
        row.clear();
        const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
        for (int v = 0; v < vocab; ++v) {
          row.push_back({logits[base + static_cast<std::size_t>(v)], v});
        }
        
        if (topk < vocab) {
          std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
          std::sort(row.begin(), row.begin() + topk, better);
        } else {
          std::sort(row.begin(), row.end(), better);
        }
        
        uint64_t local_hash = 1469598103934665603ULL;
        for (int k = 0; k < topk; ++k) {
          local_hash = mix(local_hash, static_cast<uint64_t>(static_cast<uint32_t>(row[static_cast<std::size_t>(k)].first)));
          local_hash = mix(local_hash, static_cast<uint64_t>(row[static_cast<std::size_t>(k)].second));
        }
        beam_hashes[static_cast<std::size_t>(b)] = local_hash;
      }
    }
    
    for (int b = 0; b < beams; ++b) {
      hash = mix(hash, beam_hashes[static_cast<std::size_t>(b)]);
    }
#else
    std::vector<std::pair<int32_t, int>> row;
    row.reserve(static_cast<std::size_t>(vocab));
    
    for (int b = 0; b < beams; ++b) {
      row.clear();
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      for (int v = 0; v < vocab; ++v) {
        row.push_back({logits[base + static_cast<std::size_t>(v)], v});
      }
      
      if (topk < vocab) {
        std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
        std::sort(row.begin(), row.begin() + topk, better);
      } else {
        std::sort(row.begin(), row.end(), better);
      }
      
      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[static_cast<std::size_t>(k)].first)));
        hash = mix(hash, static_cast<uint64_t>(row[static_cast<std::size_t>(k)].second));
      }
    }
#endif
  }
  return hash;
}