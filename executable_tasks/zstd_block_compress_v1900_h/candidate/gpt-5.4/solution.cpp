#include "interface.h"

#include "zstd.h"

#include <string>

int compress_payload(const std::string& input, std::string& compressed) {
  const size_t input_size = input.size();
  const size_t bound = ZSTD_compressBound(input_size);
  compressed.resize(bound);

  // Favor throughput over ratio: the baseline used level 19, which is very slow.
  // Level 1 provides much higher compression speed while preserving lossless
  // roundtrip correctness required by the task.
  const size_t n = ZSTD_compress(
      compressed.data(), bound, input.data(), input_size, 1);

  if (ZSTD_isError(n)) {
    compressed.clear();
    return -1;
  }

  compressed.resize(n);
  return static_cast<int>(n);
}
