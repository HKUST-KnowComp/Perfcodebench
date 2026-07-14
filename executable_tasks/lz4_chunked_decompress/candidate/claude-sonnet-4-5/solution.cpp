#include "interface.h"

#include "lz4.h"

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
  uint64_t sum = 0;
  std::string restored;
  
  // Reserve capacity for the largest chunk to minimize reallocations
  if (!sizes.empty()) {
    int max_size = sizes[0];
    for (int size : sizes) {
      if (size > max_size) max_size = size;
    }
    restored.reserve(static_cast<std::size_t>(max_size));
  }
  
  for (std::size_t i = 0; i < chunks.size(); ++i) {
    const std::string& local = chunks[i];  // Use const reference instead of copy
    const int expected_size = sizes[i];
    restored.resize(static_cast<std::size_t>(expected_size));
    
    const int decoded = LZ4_decompress_safe(
        local.data(), restored.data(), 
        static_cast<int>(local.size()), 
        expected_size);
    
    if (decoded != expected_size) {
      return 0;
    }
    
    // Compute checksum with loop unrolling for better ILP
    const unsigned char* data = reinterpret_cast<const unsigned char*>(restored.data());
    std::size_t len = static_cast<std::size_t>(expected_size);
    std::size_t j = 0;
    
    // Process 8 bytes at a time
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    uint64_t sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0;
    
    for (; j + 7 < len; j += 8) {
      sum0 += data[j];
      sum1 += data[j + 1];
      sum2 += data[j + 2];
      sum3 += data[j + 3];
      sum4 += data[j + 4];
      sum5 += data[j + 5];
      sum6 += data[j + 6];
      sum7 += data[j + 7];
    }
    
    sum += sum0 + sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7;
    
    // Handle remaining bytes
    for (; j < len; ++j) {
      sum += data[j];
    }
  }
  
  return sum;
}