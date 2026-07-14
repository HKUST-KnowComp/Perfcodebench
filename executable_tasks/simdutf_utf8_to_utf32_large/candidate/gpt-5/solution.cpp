#include "interface.h"
#include "simdutf.h"

uint64_t transcode_checksum(const std::string& input) {
  const char* buf = input.data();
  const size_t len = input.size();

  // FNV-1a 64-bit offset basis and prime
  uint64_t hash = 1469598103934665603ULL;
  if (len == 0) {
    return hash;
  }

  // Maximum number of UTF-32 code points is bounded by the number of UTF-8 bytes
  char32_t* out = new char32_t[len];
  size_t written = simdutf::convert_valid_utf8_to_utf32(buf, len, out);

  const char32_t* p = out;
  const char32_t* e = out + written;

  // Unroll hashing for better throughput
  const uint64_t FNV_PRIME = 1099511628211ULL;
  while (p + 4 <= e) {
    hash ^= static_cast<uint64_t>(p[0]);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[1]);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[2]);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[3]);
    hash *= FNV_PRIME;
    p += 4;
  }
  while (p < e) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= FNV_PRIME;
  }

  delete[] out;
  return hash;
}
