#include "interface.h"
#include <cstdint>
#include <string>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t update_hash(uint64_t hash, char ch) {
  hash ^= static_cast<uint64_t>(static_cast<unsigned char>(ch));
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = input.size();
  
  for (size_t i = 0; i < n; i += 3) {
    const uint32_t b0 = static_cast<unsigned char>(input[i]);
    const uint32_t b1 = (i + 1 < n) ? static_cast<unsigned char>(input[i + 1]) : 0U;
    const uint32_t b2 = (i + 2 < n) ? static_cast<unsigned char>(input[i + 2]) : 0U;
    
    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
    
    hash = update_hash(hash, kTable[(triple >> 18) & 0x3FU]);
    hash = update_hash(hash, kTable[(triple >> 12) & 0x3FU]);
    
    if (i + 1 < n) {
      hash = update_hash(hash, kTable[(triple >> 6) & 0x3FU]);
    } else {
      hash = update_hash(hash, '=');
    }
    
    if (i + 2 < n) {
      hash = update_hash(hash, kTable[triple & 0x3FU]);
    } else {
      hash = update_hash(hash, '=');
    }
  }
  
  return hash;
}