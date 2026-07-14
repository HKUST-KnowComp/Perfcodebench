#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  omp_set_num_threads(num_threads());
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    
    // Create private histograms for each thread
    std::vector<std::vector<uint32_t>> private_counts(
        omp_get_max_threads(), 
        std::vector<uint32_t>(static_cast<std::size_t>(bins), 0));
    
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        
        // Each thread processes its own subset of input
        #pragma omp for schedule(static)
        for (std::size_t i = 0; i < input.size(); ++i) {
            private_counts[tid][static_cast<std::size_t>(input[i])]++;
        }
    }
    
    // Merge private histograms into final counts
    for (int t = 0; t < omp_get_num_threads(); ++t) {
        for (int b = 0; b < bins; ++b) {
            counts[b] += private_counts[t][b];
        }
    }
  }
  
  return checksum_u32(counts);
}