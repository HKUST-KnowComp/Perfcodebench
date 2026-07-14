#include "interface.h"
#include "zstd.h"
#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  if (input.empty()) {
    compressed.clear();
    return 0;
  }

  size_t bound = ZSTD_compressBound(input.size());
  compressed.resize(bound);

  // Level 3 is the Zstandard default and provides the best balance of speed and ratio.
  // Level 19 is extremely slow and intended for archival purposes, not general throughput.
  size_t n = ZSTD_compress(
      compressed.data(), 
      compressed.size(), 
      input.data(), 
      input.size(), 
      3
  );

  if (ZSTD_isError(n)) {
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}