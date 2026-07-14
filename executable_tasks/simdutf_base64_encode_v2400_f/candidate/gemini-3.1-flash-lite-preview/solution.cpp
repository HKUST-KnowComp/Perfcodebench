#include "interface.h"
#include <string>
#include <vector>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

}  // namespace

uint64_t encode_checksum(const std::string& input) {
  const size_t n = input.size();
  if (n == 0) return 1469598103934665603ULL;

  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* data = reinterpret_cast<const uint8_t*>(input.data());
  
  auto update_hash = [&](uint8_t ch) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  };

  size_t i = 0;
  for (; i + 2 < n; i += 3) {
    uint32_t triple = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
    uint8_t c0 = kTable[(triple >> 18) & 0x3F];
    uint8_t c1 = kTable[(triple >> 12) & 0x3F];
    uint8_t c2 = kTable[(triple >> 6) & 0x3F];
    uint8_t c3 = kTable[triple & 0x3F];
    update_hash(c0); update_hash(c1); update_hash(c2); update_hash(c3);
  }

  if (i < n) {
    uint32_t b0 = data[i];
    uint32_t b1 = (i + 1 < n) ? data[i + 1] : 0;
    uint32_t b2 = 0;
    uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
    
    update_hash(kTable[(triple >> 18) & 0x3F]);
    update_hash(kTable[(triple >> 12) & 0x3F]);
    if (i + 1 < n) {
      update_hash(kTable[(triple >> 6) & 0x3F]);
    } else {
      update_hash('=');
    }
    update_hash('=');
  }

  return hash;
}