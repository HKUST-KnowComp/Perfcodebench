#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  using namespace rapidjson;

  Document d;
  // Parse using pointer and length to avoid strlen and handle non-null-terminated buffers robustly
  d.Parse(json_input.data(), static_cast<size_t>(json_input.size()));

  uint64_t sum = 0;
  if (!d.IsArray()) {
    return sum;
  }

  auto arr = d.GetArray();

  // Use StringRef with explicit lengths to avoid strlen during lookups
  const StringRef idKey("id", 2);
  const StringRef nestedKey("nested", 6);
  const StringRef aKey("a", 1);
  const StringRef bKey("b", 1);

  for (auto& item : arr) {
    const auto& nested = item[nestedKey];
    sum += item[idKey].GetUint64();
    sum += nested[aKey].GetUint64();
    sum += nested[bKey].GetUint64();
  }

  return sum;
}
