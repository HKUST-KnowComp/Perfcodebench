#include "interface.h"

#include <cstdint>
#include <string>

namespace {

// Standard Base64 table
constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// FNV-1a 64-bit parameters
constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime  = 1099511628211ULL;

inline void fnv_update(uint64_t &h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= kFNVPrime;
}

inline void process_triple(const unsigned char *p, uint64_t &hash) {
  const uint32_t b0 = p[0];
  const uint32_t b1 = p[1];
  const uint32_t b2 = p[2];

  const uint32_t o0 = (b0 >> 2);
  const uint32_t o1 = ((b0 & 0x03u) << 4) | (b1 >> 4);
  const uint32_t o2 = ((b1 & 0x0Fu) << 2) | (b2 >> 6);
  const uint32_t o3 = (b2 & 0x3Fu);

  fnv_update(hash, static_cast<unsigned char>(kTable[o0]));
  fnv_update(hash, static_cast<unsigned char>(kTable[o1]));
  fnv_update(hash, static_cast<unsigned char>(kTable[o2]));
  fnv_update(hash, static_cast<unsigned char>(kTable[o3]));
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const size_t n = input.size();

  uint64_t hash = kFNVOffset;
  if (n == 0) {
    return hash; // empty output -> initial FNV-1a value
  }

  const size_t full_triples = n / 3;
  const size_t rem = n - full_triples * 3;
  const unsigned char* pend = p + full_triples * 3;

  // Unroll by 4 triples (12 bytes) when possible
  const unsigned char* pu = p;
  while (pu + 12 <= pend) {
    process_triple(pu + 0, hash);
    process_triple(pu + 3, hash);
    process_triple(pu + 6, hash);
    process_triple(pu + 9, hash);
    pu += 12;
  }
  // Remaining full triples
  while (pu < pend) {
    process_triple(pu, hash);
    pu += 3;
  }

  // Handle remainder with padding
  if (rem == 1) {
    const uint32_t b0 = p[full_triples * 3 + 0];
    const uint32_t o0 = (b0 >> 2);
    const uint32_t o1 = ((b0 & 0x03u) << 4);

    fnv_update(hash, static_cast<unsigned char>(kTable[o0]));
    fnv_update(hash, static_cast<unsigned char>(kTable[o1]));
    fnv_update(hash, static_cast<unsigned char>('='));
    fnv_update(hash, static_cast<unsigned char>('='));
  } else if (rem == 2) {
    const uint32_t b0 = p[full_triples * 3 + 0];
    const uint32_t b1 = p[full_triples * 3 + 1];
    const uint32_t o0 = (b0 >> 2);
    const uint32_t o1 = ((b0 & 0x03u) << 4) | (b1 >> 4);
    const uint32_t o2 = ((b1 & 0x0Fu) << 2);

    fnv_update(hash, static_cast<unsigned char>(kTable[o0]));
    fnv_update(hash, static_cast<unsigned char>(kTable[o1]));
    fnv_update(hash, static_cast<unsigned char>(kTable[o2]));
    fnv_update(hash, static_cast<unsigned char>('='));
  }

  return hash;
}
