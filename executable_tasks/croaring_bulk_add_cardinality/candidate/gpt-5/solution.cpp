#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
  const size_t n = values.size();
  if (n == 0) return 0;

  const uint32_t* p = values.data();
  const uint32_t* const end = p + n;

  uint64_t count = 1; // at least one unique when non-empty
  uint32_t prev = *p++;
  while (p != end) {
    const uint32_t v = *p++;
    count += static_cast<uint64_t>(v != prev);
    prev = v;
  }
  return count;
}
