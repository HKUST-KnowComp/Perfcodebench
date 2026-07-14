#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
inline void fnv1a_update(uint64_t &hash, uint8_t byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= 1099511628211ULL; // FNV-1a 64-bit prime
}
}

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
  const char32_t* ptr = input.data();
  const size_t len = input.size();

  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  for (size_t i = 0; i < len; i++) {
    uint32_t cp = static_cast<uint32_t>(ptr[i]);
    if (cp <= 0x7F) {
      fnv1a_update(hash, static_cast<uint8_t>(cp));
    } else if (cp <= 0x7FF) {
      fnv1a_update(hash, static_cast<uint8_t>(0xC0u | (cp >> 6)));
      fnv1a_update(hash, static_cast<uint8_t>(0x80u | (cp & 0x3Fu)));
    } else if (cp <= 0xFFFF) {
      fnv1a_update(hash, static_cast<uint8_t>(0xE0u | (cp >> 12)));
      fnv1a_update(hash, static_cast<uint8_t>(0x80u | ((cp >> 6) & 0x3Fu)));
      fnv1a_update(hash, static_cast<uint8_t>(0x80u | (cp & 0x3Fu)));
    } else {
      // cp is assumed valid (<= 0x10FFFF) per task contract
      fnv1a_update(hash, static_cast<uint8_t>(0xF0u | (cp >> 18)));
      fnv1a_update(hash, static_cast<uint8_t>(0x80u | ((cp >> 12) & 0x3Fu)));
      fnv1a_update(hash, static_cast<uint8_t>(0x80u | ((cp >> 6) & 0x3Fu)));
      fnv1a_update(hash, static_cast<uint8_t>(0x80u | (cp & 0x3Fu)));
    }
  }

  return hash;
}
