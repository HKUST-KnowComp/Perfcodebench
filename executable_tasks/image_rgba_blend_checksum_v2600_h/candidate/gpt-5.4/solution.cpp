#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

struct BlendTable {
  uint8_t v[256][256];

  BlendTable() {
    for (unsigned x = 0; x < 256; ++x) {
      for (unsigned a = 0; a < 256; ++a) {
        v[x][a] = static_cast<uint8_t>((x * a + 127u) / 255u);
      }
    }
  }
};

inline const BlendTable& get_blend_table() {
  static const BlendTable table;
  return table;
}

inline void fnv1a_add(uint64_t& hash, uint8_t b) {
  hash ^= static_cast<uint64_t>(b);
  hash *= 1099511628211ULL;
}

}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;

  const std::size_t n = src.size();
  if (n == 0 || iters <= 0) {
    return 0;
  }

  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  const uint8_t* const end = s + n;
  const BlendTable& tbl = get_blend_table();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    const uint8_t* ps = s;
    const uint8_t* pd = d;

    while (ps != end) {
      const uint8_t sr = ps[0];
      const uint8_t sg = ps[1];
      const uint8_t sb = ps[2];
      const uint8_t a = ps[3];

      if (a == 0) {
        fnv1a_add(h, pd[0]);
        fnv1a_add(h, pd[1]);
        fnv1a_add(h, pd[2]);
        fnv1a_add(h, 255u);
      } else if (a == 255) {
        fnv1a_add(h, sr);
        fnv1a_add(h, sg);
        fnv1a_add(h, sb);
        fnv1a_add(h, 255u);
      } else {
        const uint8_t ia = static_cast<uint8_t>(255u - a);
        fnv1a_add(h, static_cast<uint8_t>(tbl.v[sr][a] + tbl.v[pd[0]][ia]));
        fnv1a_add(h, static_cast<uint8_t>(tbl.v[sg][a] + tbl.v[pd[1]][ia]));
        fnv1a_add(h, static_cast<uint8_t>(tbl.v[sb][a] + tbl.v[pd[2]][ia]));
        fnv1a_add(h, 255u);
      }

      ps += 4;
      pd += 4;
    }

    hash = h;
  }

  return hash;
}
