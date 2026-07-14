#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

namespace {

static inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  const size_t in_len = input.size();
  if (in_len == 0) {
    return 1469598103934665603ULL;
  }

  std::string decoded;
  decoded.resize(simdutf::maximal_binary_length_from_base64(input.data(), in_len));

  size_t written = 0;
  simdutf::error_code err = simdutf::base64_to_binary(
      input.data(), in_len, decoded.data(), written);

  if (err != simdutf::SUCCESS) {
    // Fallback scalar decoder for robustness on any valid standard base64 input.
    static constexpr signed char table[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,
        -1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    };

    decoded.resize((in_len / 4) * 3);
    size_t out = 0;
    for (size_t i = 0; i < in_len; i += 4) {
      const int c0 = table[static_cast<unsigned char>(input[i])];
      const int c1 = table[static_cast<unsigned char>(input[i + 1])];
      const int c2 = table[static_cast<unsigned char>(input[i + 2])];
      const int c3 = table[static_cast<unsigned char>(input[i + 3])];
      const uint32_t triple =
          (static_cast<uint32_t>(c0) << 18) |
          (static_cast<uint32_t>(c1) << 12) |
          (static_cast<uint32_t>((c2 < 0) ? 0 : c2) << 6) |
          static_cast<uint32_t>((c3 < 0) ? 0 : c3);
      decoded[out++] = static_cast<char>((triple >> 16) & 0xFFU);
      if (c2 != -2) decoded[out++] = static_cast<char>((triple >> 8) & 0xFFU);
      if (c3 != -2) decoded[out++] = static_cast<char>(triple & 0xFFU);
    }
    written = out;
  }

  return checksum_bytes(decoded.data(), written);
}
