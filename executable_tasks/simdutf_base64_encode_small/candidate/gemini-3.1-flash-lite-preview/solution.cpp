#include "interface.h"
#include <string>
#include <vector>

namespace {
constexpr char kTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t fnv_step(uint64_t hash, char ch) {
  return (hash ^ static_cast<uint64_t>(static_cast<unsigned char>(ch))) * 1099511628211ULL;
}
} // namespace

uint64_t encode_checksum(const std::string& input) {
  uint64_t hash = 1469598103934665603ULL;
  size_t n = input.size();
  size_t i = 0;

  for (; i + 2 < n; i += 3) {
    uint32_t b = (static_cast<unsigned char>(input[i]) << 16) | 
                 (static_cast<unsigned char>(input[i+1]) << 8) | 
                 (static_cast<unsigned char>(input[i+2]));
    hash = fnv_step(hash, kTable[(b >> 18) & 0x3F]);
    hash = fnv_step(hash, kTable[(b >> 12) & 0x3F]);
    hash = fnv_step(hash, kTable[(b >> 6) & 0x3F]);
    hash = fnv_step(hash, kTable[b & 0x3F]);
  }

  if (i < n) {
    uint32_t b0 = static_cast<unsigned char>(input[i]);
    uint32_t b1 = (i + 1 < n) ? static_cast<unsigned char>(input[i + 1]) : 0;
    uint32_t b2 = 0;
    
    hash = fnv_step(hash, kTable[(b0 >> 2) & 0x3F]);
    hash = fnv_step(hash, kTable[((b0 << 4) | (b1 >> 4)) & 0x3F]);
    
    if (i + 1 < n) {
      hash = fnv_step(hash, kTable[(b1 << 2) & 0x3F]);
      hash = fnv_step(hash, '=');
    } else {
      hash = fnv_step(hash, '=');
      hash = fnv_step(hash, '=');
    }
  }

  return hash;
}