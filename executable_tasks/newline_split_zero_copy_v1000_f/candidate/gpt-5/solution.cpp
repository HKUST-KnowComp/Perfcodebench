#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  if (iters <= 0) return 0;

  const char* data = text.data();
  const char* const end = data + text.size();

  uint64_t hash = 1469598103934665603ULL;

  const char* line_start = data;
  for (const char* p = data; p < end; ++p) {
    if (*p == '\n') {
      const size_t len = static_cast<size_t>(p - line_start);
      hash = mix(hash, static_cast<uint64_t>(len));
      const unsigned char first = (len > 0) ? static_cast<unsigned char>(line_start[0]) : 0u;
      hash = mix(hash, static_cast<uint64_t>(first));
      line_start = p + 1;
    }
  }

  if (line_start < end) {
    const size_t len = static_cast<size_t>(end - line_start);
    hash = mix(hash, static_cast<uint64_t>(len));
    const unsigned char first = (len > 0) ? static_cast<unsigned char>(line_start[0]) : 0u;
    hash = mix(hash, static_cast<uint64_t>(first));
  }

  return hash;
}
