#include "interface.h"
#include "simdutf.h"

namespace {

inline uint64_t fnv1a64(const unsigned char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL; // FNV offset basis
  const unsigned char* p = data;
  const unsigned char* end = data + len;
  const uint64_t prime = 1099511628211ULL; // FNV prime

  // Unroll by 8 for throughput on large buffers
  while (p + 8 <= end) {
    hash ^= static_cast<uint64_t>(p[0]); hash *= prime;
    hash ^= static_cast<uint64_t>(p[1]); hash *= prime;
    hash ^= static_cast<uint64_t>(p[2]); hash *= prime;
    hash ^= static_cast<uint64_t>(p[3]); hash *= prime;
    hash ^= static_cast<uint64_t>(p[4]); hash *= prime;
    hash ^= static_cast<uint64_t>(p[5]); hash *= prime;
    hash ^= static_cast<uint64_t>(p[6]); hash *= prime;
    hash ^= static_cast<uint64_t>(p[7]); hash *= prime;
    p += 8;
  }
  while (p < end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= prime;
  }
  return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
  // Fast path for empty input
  if (input.empty()) {
    return 1469598103934665603ULL; // FNV-1a offset basis for empty sequence
  }

  // Compute exact UTF-8 output length and allocate raw buffer to avoid zero-initialization
  const size_t out_len = simdutf::utf8_length_from_utf16(input.data(), input.size());
  if (out_len == 0) {
    // Valid empty output (e.g., input empty) or pathological; checksum of empty
    return 1469598103934665603ULL;
  }

  char* out = new char[out_len];

  // Input guaranteed valid per task; use the faster valid conversion path
  const size_t written = simdutf::convert_valid_utf16_to_utf8(input.data(), input.size(), out);

  // Compute checksum over exactly the produced bytes
  const uint64_t hash = fnv1a64(reinterpret_cast<const unsigned char*>(out), written);

  delete[] out;
  return hash;
}
