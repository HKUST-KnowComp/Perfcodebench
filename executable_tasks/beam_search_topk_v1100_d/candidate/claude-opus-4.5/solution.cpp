#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <cstdint>

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
  // Pre-allocate storage for each beam's top-k results
  std::vector<std::vector<std::pair<int32_t, int>>> beam_results(static_cast<size_t>(beams));
  for (int b = 0; b < beams; ++b) {
    beam_results[static_cast<size_t>(b)].reserve(static_cast<size_t>(topk));
  }
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // Process each beam
    for (int b = 0; b < beams; ++b) {
      const int32_t* beam_logits = logits.data() + static_cast<size_t>(b) * static_cast<size_t>(vocab);
      auto& results = beam_results[static_cast<size_t>(b)];
      results.clear();
      
      // Use a min-heap approach: maintain top-k elements
      // For small k, this is more efficient than partial_sort
      if (topk <= 0) continue;
      
      // Initialize with first topk elements
      for (int v = 0; v < topk && v < vocab; ++v) {
        results.push_back({beam_logits[v], v});
      }
      
      // Make it a min-heap based on our "better" comparison
      // We want the "worst" of our top-k at the top so we can replace it
      auto worse = [](const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
        return better(a, b);  // Inverted for min-heap behavior
      };
      std::make_heap(results.begin(), results.end(), worse);
      
      // Process remaining elements
      for (int v = topk; v < vocab; ++v) {
        std::pair<int32_t, int> candidate = {beam_logits[v], v};
        // If candidate is better than the worst in our heap, replace it
        if (better(candidate, results[0])) {
          std::pop_heap(results.begin(), results.end(), worse);
          results.back() = candidate;
          std::push_heap(results.begin(), results.end(), worse);
        }
      }
      
      // Sort the top-k results to get them in order
      std::sort(results.begin(), results.end(), better);
      
      // Compute hash for this beam
      for (int k = 0; k < topk && k < static_cast<int>(results.size()); ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(results[static_cast<size_t>(k)].first)));
        hash = mix(hash, static_cast<uint64_t>(results[static_cast<size_t>(k)].second));
      }
    }
  }
  
  return hash;
}