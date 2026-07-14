#include "interface.h"

namespace {

static inline void fnv1a_process(uint64_t& h, uint8_t b) {
  h ^= static_cast<uint64_t>(b);
  h *= 1099511628211ULL; // FNV-1a 64-bit prime
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0;

  uint64_t h = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  for (const std::string& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    fnv1a_process(h, static_cast<uint8_t>(len & 0xFF));
    fnv1a_process(h, static_cast<uint8_t>((len >> 8) & 0xFF));

    const unsigned char* p = reinterpret_cast<const unsigned char*>(frame.data());
    size_t n = frame.size();

    // Unroll processing by 8 for throughput
    size_t blocks = n / 8;
    while (blocks--) {
      fnv1a_process(h, *p++);
      fnv1a_process(h, *p++);
      fnv1a_process(h, *p++);
      fnv1a_process(h, *p++);
      fnv1a_process(h, *p++);
      fnv1a_process(h, *p++);
      fnv1a_process(h, *p++);
      fnv1a_process(h, *p++);
    }
    switch (n & 7) {
      case 7: fnv1a_process(h, *p++);
      case 6: fnv1a_process(h, *p++);
      case 5: fnv1a_process(h, *p++);
      case 4: fnv1a_process(h, *p++);
      case 3: fnv1a_process(h, *p++);
      case 2: fnv1a_process(h, *p++);
      case 1: fnv1a_process(h, *p++);
      default: break;
    }
  }

  return h;
}
