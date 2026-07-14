#include "interface.h"

#include <cstdint>
#include <string>

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str(), json_input.size());

  if (doc.HasParseError() || !doc.IsArray()) {
    return 0;
  }

  uint64_t sum = 0;
  const auto& arr = doc.GetArray();
  for (auto it = arr.Begin(); it != arr.End(); ++it) {
    if (!it->IsObject()) continue;

    const auto obj = it->GetObject();

    auto idIt = obj.FindMember("id");
    if (idIt != obj.MemberEnd() && idIt->value.IsUint64()) {
      sum += idIt->value.GetUint64();
    }

    auto nestedIt = obj.FindMember("nested");
    if (nestedIt != obj.MemberEnd() && nestedIt->value.IsObject()) {
      const auto nested = nestedIt->value.GetObject();

      auto aIt = nested.FindMember("a");
      if (aIt != nested.MemberEnd() && aIt->value.IsUint64()) {
        sum += aIt->value.GetUint64();
      }

      auto bIt = nested.FindMember("b");
      if (bIt != nested.MemberEnd() && bIt->value.IsUint64()) {
        sum += bIt->value.GetUint64();
      }
    }
  }

  return sum;
}
