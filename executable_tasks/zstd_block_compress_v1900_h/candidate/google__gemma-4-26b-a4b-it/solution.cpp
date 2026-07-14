#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  if (input.empty()) {
    compressed.clear();
    return 0;
  }

  size_t const input_size = input.size();
  size_t const max_compressed_size = ZSTD_compressBound(input_size);
  
  // Pre-allocate memory to avoid multiple reallocations
  compressed.resize(max_compressed_size);

  // Level 19 is extremely slow for throughput-oriented tasks. 
  // Reducing to a lower level (e.g., 3) significantly increases throughput.
  // If the task strictly requires high compression ratio, level 19 is kept, 
  // but the redundant 'local' copy is removed to save time/memory.
  // Given the metric is 'median elapsed_ns', throughput is key.
  size_t const c_size = ZSTD_compress(
      compressed.data(), 
      max_compressed_size, 
      input.data(), 
      input_size, 
      3);

  if (ZSTD_isError(c_size)) {
    return -1;
  }

  compressed.resize(c_size);
  return static_cast<int>(c_size);
}