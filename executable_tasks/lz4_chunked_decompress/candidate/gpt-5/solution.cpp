#include "interface.h"

#include "lz4.h"

#include <algorithm>
#include <vector>

uint64_t decompress_checksum(const std::vector<std::string>& chunks, const std::vector<int>& sizes) {
  const std::size_t n = chunks.size();
  if (sizes.size() != n) return 0;
  if (n == 0) return 0;

  // Determine maximum output size to allocate once and reuse
  int max_out = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (sizes[i] < 0) return 0;
    if (sizes[i] > max_out) max_out = sizes[i];
  }
  if (max_out <= 0) return 0;

  std::vector<char> out(static_cast<std::size_t>(max_out));

  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    const std::string& in = chunks[i];
    const int out_len = sizes[i];

    const int decoded = LZ4_decompress_safe(
        in.data(), out.data(), static_cast<int>(in.size()), out_len);
    if (decoded != out_len) {
      return 0;
    }

    const unsigned char* p = reinterpret_cast<const unsigned char*>(out.data());
    const unsigned char* end = p + static_cast<std::size_t>(decoded);

    // Unrolled accumulation for better throughput
    while (p + 8 <= end) {
      sum += static_cast<uint64_t>(p[0])
           + static_cast<uint64_t>(p[1])
           + static_cast<uint64_t>(p[2])
           + static_cast<uint64_t>(p[3])
           + static_cast<uint64_t>(p[4])
           + static_cast<uint64_t>(p[5])
           + static_cast<uint64_t>(p[6])
           + static_cast<uint64_t>(p[7]);
      p += 8;
    }
    while (p < end) {
      sum += static_cast<uint64_t>(*p++);
    }
  }

  return sum;
}
