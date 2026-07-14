#pragma once

#include <cstdint>
#include <vector>
#include <omp.h>

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

struct Histogram {
  std::vector<uint32_t> counts;
  Histogram(int bins) : counts(bins, 0) {}
  void operator+=(const Histogram& other) {
    for (size_t i = 0; i < counts.size(); ++i) {
      counts[i] += other.counts[i];
    }
  }
};

#pragma omp declare reduction (hist_sum: Histogram: omp_out += omp_in)

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
  omp_set_num_threads(num_threads());
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    
    Histogram local_counts(bins);
    
    #pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < input.size(); ++i) {
      local_counts.counts[static_cast<std::size_t>(input[i])]++;
    }
    
    #pragma omp parallel
    {
      #pragma omp single
      {
        for (int j = 0; j < bins; ++j) {
          counts[j] += local_counts.counts[j];
        }
      }
    }
  }
  
  return checksum_u32(counts);
}