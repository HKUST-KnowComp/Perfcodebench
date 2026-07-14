#include "interface.h"

#include "lz4.h"
#include <algorithm>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
  if (chunks.empty()) {
    return 0;
  }

  // Find max size to pre-allocate buffer once
  int max_size = *std::max_element(sizes.begin(), sizes.end());
  
  // Use a single pre-allocated buffer for all decompressions
  std::vector<uint8_t> buffer(static_cast<size_t>(max_size));
  
  uint64_t sum = 0;
  const size_t n = chunks.size();
  
  for (size_t i = 0; i < n; ++i) {
    const std::string& chunk = chunks[i];
    const int expected_size = sizes[i];
    
    const int decoded = LZ4_decompress_safe(
        chunk.data(),
        reinterpret_cast<char*>(buffer.data()),
        static_cast<int>(chunk.size()),
        expected_size);
    
    if (decoded != expected_size) {
      return 0;
    }
    
    // Compute checksum with loop unrolling
    const uint8_t* ptr = buffer.data();
    const size_t len = static_cast<size_t>(decoded);
    size_t j = 0;
    
    // Process 8 bytes at a time
    const size_t unroll_limit = len & ~static_cast<size_t>(7);
    for (; j < unroll_limit; j += 8) {
      sum += ptr[j];
      sum += ptr[j + 1];
      sum += ptr[j + 2];
      sum += ptr[j + 3];
      sum += ptr[j + 4];
      sum += ptr[j + 5];
      sum += ptr[j + 6];
      sum += ptr[j + 7];
    }
    
    // Handle remaining bytes
    for (; j < len; ++j) {
      sum += ptr[j];
    }
  }
  
  return sum;
}