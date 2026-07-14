#include "interface.h"
#include <string>
#include <vector>
#include "simdutf.h"

uint64_t transcode_checksum(const std::u16string& input) {
  std::vector<char> output(input.size() * 4);
  size_t result = simdutf::utf16_to_utf8(input.data(), input.size(), output.data());
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < result; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(output[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}