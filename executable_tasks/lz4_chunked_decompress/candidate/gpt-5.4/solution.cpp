#include "interface.h"

#include "lz4.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
  const std::size_t n = chunks.size();
  if (n != sizes.size()) {
    return 0;
  }

  uint64_t sum = 0;
  std::vector<char> buffer;

  for (std::size_t i = 0; i < n; ++i) {
    const int out_size = sizes[i];
    if (out_size < 0) {
      return 0;
    }

    const std::size_t needed = static_cast<std::size_t>(out_size);
    if (buffer.size() < needed) {
      buffer.resize(needed);
    }

    const std::string& chunk = chunks[i];
    const int compressed_size = static_cast<int>(chunk.size());
    const int decoded = LZ4_decompress_safe(
        chunk.data(),
        buffer.data(),
        compressed_size,
        out_size);

    if (decoded != out_size) {
      return 0;
    }

    const unsigned char* p = reinterpret_cast<const unsigned char*>(buffer.data());
    const unsigned char* end = p + needed;
    uint64_t local_sum = 0;
    while (p != end) {
      local_sum += static_cast<uint64_t>(*p++);
    }
    sum += local_sum;
  }

  return sum;
}
