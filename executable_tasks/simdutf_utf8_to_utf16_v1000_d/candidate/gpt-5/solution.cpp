#include "interface.h"

#include <simdutf.h>
#include <cstddef>
#include <cstdint>
#include <memory>

uint64_t transcode_checksum(const std::string& input) {
  constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;

  const size_t len = input.size();
  if (len == 0) {
    return FNV_OFFSET_BASIS;
  }

  // Worst-case UTF-16 code units needed is <= input.size()
  std::unique_ptr<char16_t[]> buffer(new char16_t[len]);

  const char* in = input.data();
  const size_t written = simdutf::convert_valid_utf8_to_utf16le(in, len, buffer.get());

  // Compute FNV-1a over the UTF-16LE code units
  uint64_t hash = FNV_OFFSET_BASIS;
  const char16_t* p = buffer.get();
  const char16_t* end = p + written;

  // Unroll for better throughput
  while (p + 8 <= end) {
    hash ^= static_cast<uint64_t>(p[0]); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[1]); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[2]); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[3]); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[4]); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[5]); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[6]); hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>(p[7]); hash *= FNV_PRIME;
    p += 8;
  }
  while (p < end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= FNV_PRIME;
  }

  return hash;
}
