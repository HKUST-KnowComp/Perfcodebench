#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* data = packed.data();
  const std::size_t nbytes = packed.size();
  const std::size_t limit = nbytes - (nbytes % 3u); // process only complete 3-byte triplets
  const uint32_t thr = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    const uint8_t* p = data;
    const uint8_t* end = data + limit;

    // Unroll by 4 triplets (12 bytes) per loop
    while (p + 12 <= end) {
      // Triplet 0
      uint32_t a0 = (uint32_t)p[0] | ((uint32_t)(p[1] & 0x0Fu) << 8);
      uint32_t b0 = ((uint32_t)(p[1] >> 4)) | ((uint32_t)p[2] << 4);
      uint32_t m0a = static_cast<uint32_t>(-(int)(a0 > thr));
      uint32_t m0b = static_cast<uint32_t>(-(int)(b0 > thr));
      s += (uint64_t)(a0 & m0a);
      s += (uint64_t)(b0 & m0b);

      // Triplet 1
      uint32_t a1 = (uint32_t)p[3] | ((uint32_t)(p[4] & 0x0Fu) << 8);
      uint32_t b1 = ((uint32_t)(p[4] >> 4)) | ((uint32_t)p[5] << 4);
      uint32_t m1a = static_cast<uint32_t>(-(int)(a1 > thr));
      uint32_t m1b = static_cast<uint32_t>(-(int)(b1 > thr));
      s += (uint64_t)(a1 & m1a);
      s += (uint64_t)(b1 & m1b);

      // Triplet 2
      uint32_t a2 = (uint32_t)p[6] | ((uint32_t)(p[7] & 0x0Fu) << 8);
      uint32_t b2 = ((uint32_t)(p[7] >> 4)) | ((uint32_t)p[8] << 4);
      uint32_t m2a = static_cast<uint32_t>(-(int)(a2 > thr));
      uint32_t m2b = static_cast<uint32_t>(-(int)(b2 > thr));
      s += (uint64_t)(a2 & m2a);
      s += (uint64_t)(b2 & m2b);

      // Triplet 3
      uint32_t a3 = (uint32_t)p[9] | ((uint32_t)(p[10] & 0x0Fu) << 8);
      uint32_t b3 = ((uint32_t)(p[10] >> 4)) | ((uint32_t)p[11] << 4);
      uint32_t m3a = static_cast<uint32_t>(-(int)(a3 > thr));
      uint32_t m3b = static_cast<uint32_t>(-(int)(b3 > thr));
      s += (uint64_t)(a3 & m3a);
      s += (uint64_t)(b3 & m3b);

      p += 12;
    }

    // Process remaining triplets
    while (p < end) {
      uint32_t a = (uint32_t)p[0] | ((uint32_t)(p[1] & 0x0Fu) << 8);
      uint32_t b = ((uint32_t)(p[1] >> 4)) | ((uint32_t)p[2] << 4);
      uint32_t ma = static_cast<uint32_t>(-(int)(a > thr));
      uint32_t mb = static_cast<uint32_t>(-(int)(b > thr));
      s += (uint64_t)(a & ma);
      s += (uint64_t)(b & mb);
      p += 3;
    }

    sum = s; // return the sum from the final iteration
  }

  return sum;
}
