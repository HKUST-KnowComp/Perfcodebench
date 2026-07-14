#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  const uint32_t* a = left.data();
  const uint32_t* b = right.data();
  const size_t n = left.size();
  const size_t m = right.size();

  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* pa = a;
    const uint32_t* pb = b;
    const uint32_t* aEnd = a + n;
    const uint32_t* bEnd = b + m;

    uint64_t count = 0;

    while (pa != aEnd && pb != bEnd) {
      uint32_t va = *pa;
      uint32_t vb = *pb;
      if (va < vb) {
        ++count;
        do { ++pa; } while (pa != aEnd && *pa == va);
      } else if (vb < va) {
        ++count;
        do { ++pb; } while (pb != bEnd && *pb == vb);
      } else { // va == vb
        ++count;
        do { ++pa; } while (pa != aEnd && *pa == va);
        do { ++pb; } while (pb != bEnd && *pb == vb);
      }
    }

    while (pa != aEnd) {
      uint32_t va = *pa;
      ++count;
      do { ++pa; } while (pa != aEnd && *pa == va);
    }

    while (pb != bEnd) {
      uint32_t vb = *pb;
      ++count;
      do { ++pb; } while (pb != bEnd && *pb == vb);
    }

    result = count;
  }

  return result;
}
