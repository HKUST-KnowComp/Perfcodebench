#include "interface.h"
#include "roaring.h"

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
  roaring::Roaring r;
  r.addMany(values.size(), values.data());
  return r.cardinality();
}