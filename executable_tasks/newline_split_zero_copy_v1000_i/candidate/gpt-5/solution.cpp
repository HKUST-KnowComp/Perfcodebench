#include "interface.h"

#include <string>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  // Match baseline behavior: if iters == 0, return 0.
  if (iters <= 0) return 0;

  const char* p = text.data();
  const char* end = p + text.size();

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  while (p < end) {
    const char* q = p;
    // Scan until newline or end.
    while (q < end && *q != '\n') {
      ++q;
    }
    uint64_t len = static_cast<uint64_t>(q - p);
    hash = mix(hash, len);
    uint64_t first = (len == 0) ? 0u : static_cast<unsigned char>(*p);
    hash = mix(hash, first);

    // If newline encountered, skip it; otherwise we reached the end.
    if (q < end) {
      p = q + 1;
    } else {
      p = q;
    }
  }

  return hash;
}
