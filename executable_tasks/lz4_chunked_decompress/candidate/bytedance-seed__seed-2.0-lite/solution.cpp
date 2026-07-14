#include "interface.h"
#include "lz4.h"

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
  uint64_t sum = 0;
  if (chunks.size() != sizes.size()) return 0;
  
  // Calculate maximum required decompression size to preallocate buffer once
  size_t max_decomp_size = 0;
  for (int s : sizes) {
    if (static_cast<size_t>(s) > max_decomp_size) {
      max_decomp_size = static_cast<size_t>(s);
    }
  }
  // Preallocate single decompression buffer to avoid per-chunk heap allocations
  std::string decomp_buf(max_decomp_size, '\0');
  
  for (std::size_t i = 0; i < chunks.size(); ++i) {
    // Remove unnecessary local copy of input chunk, use original data directly
    const char* comp_data = chunks[i].data();
    const int comp_size = static_cast<int>(chunks[i].size());
    const int target_decomp_size = sizes[i];
    
    const int decoded = LZ4_decompress_safe(
        comp_data, decomp_buf.data(), comp_size, target_decomp_size);
    if (decoded != target_decomp_size) {
      return 0;
    }
    // Accumulate checksum over decompressed bytes, same as original logic
    for (int j = 0; j < target_decomp_size; ++j) {
      sum += static_cast<uint64_t>(static_cast<unsigned char>(decomp_buf[j]));
    }
  }
  return sum;
}