#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t encode_checksum(const std::string& input) {
  const std::size_t n = input.size();
  const std::size_t out_size = ((n + 2) / 3) * 4;
  std::vector<char> out(out_size);
  const std::size_t written = simdutf::binary_to_base64(input.data(), n, out.data());
  (void)written; // should equal out_size
  
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < out_size; ++i) {
    const uint64_t ch = static_cast<unsigned char>(out[i]);
    hash ^= ch;
    hash *= 1099511628211ULL;
  }
  return hash;
}