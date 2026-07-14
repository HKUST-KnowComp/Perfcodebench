#include "interface.h"
#include "simdutf.h"
#include <vector>

/**
 * Optimized implementation using simdutf for high-performance transcoding.
 * The simdutf library provides architecture-specific SIMD kernels that
 * outperform manual scalar loops significantly.
 */
uint64_t transcode_checksum(const std::string& input) {
  if (input.empty()) return 1469598103934665603ULL;

  // Pre-allocate buffer for worst-case scenario (UTF-8 to UTF-16 expansion)
  // UTF-8 to UTF-16 can expand at most 1:1 for BMP, or 1:2 for supplementary planes.
  std::vector<char16_t> out(input.size() * 2);

  simdutf::result res = simdutf::convert_utf8_to_utf16(input.data(), input.size(), out.data());
  
  // The result count is the number of char16_t written
  size_t count = res.count;

  // FNV-1a checksum calculation
  uint64_t hash = 1469598103934665603ULL;
  const char16_t* data = out.data();
  for (size_t i = 0; i < count; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }

  return hash;
}