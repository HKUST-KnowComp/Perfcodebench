#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include "simdutf.h"

namespace {

static inline uint64_t checksum_utf32(const char32_t* data, size_t length) {
  // FNV-1a 64-bit
  constexpr uint64_t FNV_offset = 1469598103934665603ULL;
  constexpr uint64_t FNV_prime  = 1099511628211ULL;
  uint64_t hash = FNV_offset;

  const char32_t* p = data;
  const char32_t* end = data + length;

  // Unroll by 8 for reduced loop overhead
  while (p + 8 <= end) {
    hash ^= static_cast<uint64_t>(p[0]);
    hash *= FNV_prime;
    hash ^= static_cast<uint64_t>(p[1]);
    hash *= FNV_prime;
    hash ^= static_cast<uint64_t>(p[2]);
    hash *= FNV_prime;
    hash ^= static_cast<uint64_t>(p[3]);
    hash *= FNV_prime;
    hash ^= static_cast<uint64_t>(p[4]);
    hash *= FNV_prime;
    hash ^= static_cast<uint64_t>(p[5]);
    hash *= FNV_prime;
    hash ^= static_cast<uint64_t>(p[6]);
    hash *= FNV_prime;
    hash ^= static_cast<uint64_t>(p[7]);
    hash *= FNV_prime;
    p += 8;
  }
  while (p < end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= FNV_prime;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  const char* src = input.data();
  const size_t len = input.size();

  // Determine exact UTF-32 length to avoid reallocations
  const size_t out_len = simdutf::utf32_length_from_utf8(src, len);
  std::vector<char32_t> out(out_len);

  // Convert; input is guaranteed valid per problem statement
  (void)simdutf::convert_utf8_to_utf32(src, len, out.data());

  return checksum_utf32(out.data(), out_len);
}
