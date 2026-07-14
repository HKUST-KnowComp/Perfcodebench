#include "interface.h"

#include <rapidjson/document.h>
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t compute_checksum(const std::string& json_input) {
  using namespace rapidjson;

  // Create a mutable, null-terminated buffer for in-situ parsing
  std::vector<char> buffer(json_input.size() + 1);
  if (!json_input.empty()) {
    std::memcpy(buffer.data(), json_input.data(), json_input.size());
  }
  buffer[json_input.size()] = '\0';

  Document doc;
  doc.ParseInsitu(buffer.data());
  if (doc.HasParseError() || !doc.IsArray()) {
    return 0;
  }

  uint64_t sum = 0;

  const StringRef idRef("id", 2);
  const StringRef nestedRef("nested", 6);
  const StringRef aRef("a", 1);
  const StringRef bRef("b", 1);

  auto arr = doc.GetArray();
  for (auto it = arr.Begin(); it != arr.End(); ++it) {
    Value& v = *it;

    auto itrId = v.FindMember(idRef);
    auto itrNested = v.FindMember(nestedRef);

    // Assuming valid input as per baseline behavior
    sum += itrId->value.GetUint64();

    Value& nested = itrNested->value;
    auto itrA = nested.FindMember(aRef);
    auto itrB = nested.FindMember(bRef);

    sum += itrA->value.GetUint64();
    sum += itrB->value.GetUint64();
  }

  return sum;
}
