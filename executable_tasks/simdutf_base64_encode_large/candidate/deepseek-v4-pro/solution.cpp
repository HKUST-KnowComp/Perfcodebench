#include "interface.h"
#include <simdutf.h>

#include <cstddef>
#include <string>

uint64_t encode_checksum(const std::string& input) {
  const std::size_t out_len = ((input.size() + 2) / 3) * 4;
  std::string out(out_len, '\0');

  const std::size_t written = simdutf::binary_to_base64(
      input.data(), input.size(), &out[0]);
  out.resize(written);

  uint64_t hash = 1469598103934665603ULL;
  for (char c : out) {
    const unsigned char ch = static_cast<unsigned char>(c);
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}
