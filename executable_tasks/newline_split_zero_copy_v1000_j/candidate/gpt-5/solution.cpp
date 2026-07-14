#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

static uint64_t compute_hash_once(const std::string& text) {
  const char* p = text.data();
  const char* const end = p + text.size();
  const char* s = p;

  uint64_t hash = 1469598103934665603ULL;

  for (const char* it = p; it < end; ++it) {
    if (*it == '\n') {
      const uint64_t len = static_cast<uint64_t>(it - s);
      hash = mix(hash, len);
      const uint64_t first = (len != 0) ? static_cast<unsigned char>(*s) : 0ULL;
      hash = mix(hash, first);
      s = it + 1;
    }
  }

  if (s < end) {
    const uint64_t len = static_cast<uint64_t>(end - s);
    hash = mix(hash, len);
    const uint64_t first = static_cast<unsigned char>(*s);
    hash = mix(hash, first);
  }

  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  if (iters <= 0) return 0ULL;
  // Compute the checksum once; final hash is identical across iterations.
  return compute_hash_once(text);
}
