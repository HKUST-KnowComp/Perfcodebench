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
    int n_threads = num_threads();
    omp_set_num_threads(n_threads);
    std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
    
    const uint8_t* data = input.data();
    size_t size = input.size();
    
    // Pad bins to a multiple of 16 (64 bytes) to prevent false sharing between thread-local histograms.
    // Ensure at least 256 bins are allocated to safely handle uint8_t values.
    int padded_bins = (std::max(256, bins) + 15) & ~15;
    std::vector<uint32_t> local_counts(n_threads * padded_bins, 0);
    
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int num_t = omp_get_num_threads();
        
        size_t chunk = size / num_t;
        size_t start = tid * chunk;
        size_t end = (tid == num_t - 1) ? size : start + chunk;
        
        uint32_t* my_counts = local_counts.data() + tid * padded_bins;
        
        for (int iter = 0; iter < iters; ++iter) {
            for (int i = 0; i < bins; ++i) {
                my_counts[i] = 0;
            }
            
            size_t i = start;
            for (; i + 7 < end; i += 8) {
                my_counts[data[i]]++;
                my_counts[data[i+1]]++;
                my_counts[data[i+2]]++;
                my_counts[data[i+3]]++;
                my_counts[data[i+4]]++;
                my_counts[data[i+5]]++;
                my_counts[data[i+6]]++;
                my_counts[data[i+7]]++;
            }
            for (; i < end; ++i) {
                my_counts[data[i]]++;
            }
            
            #pragma omp barrier
            
            #pragma omp for schedule(static)
            for (int b = 0; b < bins; ++b) {
                uint32_t sum = 0;
                for (int t = 0; t < num_t; ++t) {
                    sum += local_counts[t * padded_bins + b];
                }
                counts[b] = sum;
            }
        }
    }
    return checksum_u32(counts);
}
