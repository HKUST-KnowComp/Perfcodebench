#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  using namespace rapidjson;

  Document doc;
  // Iterative parsing reduces recursion overhead for nested structures
  doc.Parse<kParseIterativeFlag>(json_input.data(), json_input.size());

  uint64_t sum = 0;
  if (!doc.IsArray()) return sum;

  const StringRef idKey("id");
  const StringRef nestedKey("nested");
  const StringRef aKey("a");
  const StringRef bKey("b");

  const auto arr = doc.GetArray();
  for (auto& item : arr) {
    if (!item.IsObject()) continue;

    const auto idIt = item.FindMember(idKey);
    if (idIt != item.MemberEnd() && idIt->value.IsUint64()) {
      sum += idIt->value.GetUint64();
    }

    const auto nestedIt = item.FindMember(nestedKey);
    if (nestedIt != item.MemberEnd() && nestedIt->value.IsObject()) {
      const auto nestedObj = nestedIt->value.GetObject();

      const auto aIt = nestedObj.FindMember(aKey);
      if (aIt != nestedObj.MemberEnd() && aIt->value.IsUint64()) {
        sum += aIt->value.GetUint64();
      }

      const auto bIt = nestedObj.FindMember(bKey);
      if (bIt != nestedObj.MemberEnd() && bIt->value.IsUint64()) {
        sum += bIt->value.GetUint64();
      }
    }
  }

  return sum;
}
