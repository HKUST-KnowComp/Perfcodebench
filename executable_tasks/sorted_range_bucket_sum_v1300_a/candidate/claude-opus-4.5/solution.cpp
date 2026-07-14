#include "interface.h"

#include <cstdint>
#include <immintrin.h>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  const size_t num_probes = probes.size();
  const size_t num_boundaries = boundaries.size();
  
  if (num_probes == 0 || iters <= 0) {
    return 0;
  }
  
  uint64_t sum = 0;
  
  const uint32_t* probe_ptr = probes.data();
  const uint32_t* boundary_ptr = boundaries.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    if (num_boundaries == 0) {
      // All probes get bucket 0
      continue;
    }
    
    // Linear merge approach: since both are sorted, we can walk through together
    // For each probe, find how many boundaries are <= probe
    
    size_t bi = 0; // boundary index
    size_t pi = 0; // probe index
    
    // Process probes, advancing boundary pointer as needed
    while (pi < num_probes && bi < num_boundaries) {
      uint32_t probe = probe_ptr[pi];
      
      // Advance boundary index while boundary <= probe
      while (bi < num_boundaries && boundary_ptr[bi] <= probe) {
        ++bi;
      }
      
      // Now bi is the bucket index for this probe (upper_bound semantics)
      sum += bi;
      ++pi;
    }
    
    // Remaining probes are all >= all boundaries
    // Each gets bucket index = num_boundaries
    if (pi < num_probes) {
      sum += static_cast<uint64_t>(num_boundaries) * (num_probes - pi);
    }
  }
  
  return sum;
}