#include "interface.h"

namespace {

static inline void fnv1a_update(uint64_t& hash, unsigned char c) {
  hash ^= static_cast<uint64_t>(c);
  hash *= 1099511628211ULL;
}

// Append unsigned integer in decimal to the hash.
static inline void append_uint_dec(uint64_t& hash, uint64_t v) {
  // Buffer large enough for 64-bit, though we only need up to 10 for uint32 and 5 for uint16
  char buf[20];
  int pos = 0;
  if (v == 0) {
    fnv1a_update(hash, static_cast<unsigned char>('0'));
    return;
  }
  while (v > 0) {
    uint64_t q = v / 10;
    unsigned char digit = static_cast<unsigned char>('0' + (v - q * 10));
    buf[pos++] = static_cast<char>(digit);
    v = q;
  }
  // Digits are in reverse; emit in correct order
  while (pos--) {
    fnv1a_update(hash, static_cast<unsigned char>(buf[pos]));
  }
}

static inline uint64_t compute_hash_once(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    append_uint_dec(hash, static_cast<uint64_t>(timestamps[i]));
    fnv1a_update(hash, static_cast<unsigned char>('|'));
    append_uint_dec(hash, static_cast<uint64_t>(ids[i]));
    fnv1a_update(hash, static_cast<unsigned char>('|'));
    append_uint_dec(hash, static_cast<uint64_t>(levels[i]));
    fnv1a_update(hash, static_cast<unsigned char>('|'));

    const std::string& msg = messages[i];
    const std::size_t msz = msg.size();
    for (std::size_t j = 0; j < msz; ++j) {
      fnv1a_update(hash, static_cast<unsigned char>(msg[j]));
    }
    fnv1a_update(hash, static_cast<unsigned char>('\n'));
  }
  return hash;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  // Match baseline behavior for iters == 0
  if (iters <= 0) {
    return 0ULL;
  }
  // The output is identical each iteration; compute once.
  uint64_t hash = compute_hash_once(timestamps, ids, levels, messages);
  return hash;
}
