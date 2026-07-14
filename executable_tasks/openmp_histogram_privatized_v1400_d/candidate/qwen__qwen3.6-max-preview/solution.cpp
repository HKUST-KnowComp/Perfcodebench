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
  int nthreads = num_threads();
  omp_set_num_threads(nthreads);
  
  std::size_t nbins = static_cast<std::size_t>(bins);
  std::vector<uint32_t> local_counts(nthreads * nbins, 0);
  std::vector<uint32_t> counts(nbins, 0);

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    uint32_t* my_counts = &local_counts[tid * nbins];
    
    for (int iter = 0; iter < iters; ++iter) {
      for (std::size_t b = 0; b < nbins; ++b) my_counts[b] = 0;
      
      #pragma omp for schedule(static)
      for (std::size_t i = 0; i < input.size(); ++i) {
        my_counts[static_cast<std::size_t>(input[i])]++;
      }
      
      #pragma omp single
      {
        std::fill(counts.begin(), counts.end(), 0u);
        for (int t = 0; t < nthreads; ++t) {
          for (std::size_t b = 0; b < nbins; ++b) {
            counts[b] += local_counts[static_cast<std::size_t>(t) * nbins + b];
          }
        }
      }
    }
  }
  return checksum_u32(counts);
}
