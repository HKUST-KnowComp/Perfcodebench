#include "interface.h"

#include <cstdint>
#include <string>

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());

  if (doc.HasParseError() || !doc.IsArray()) {
    return 0;
  }

  uint64_t sum = 0;
  for (auto it = doc.Begin(); it != doc.End(); ++it) {
    const rapidjson::Value& item = *it;
    if (!item.IsObject()) continue;

    auto idIt = item.FindMember("id");
    if (idIt != item.MemberEnd() && idIt->value.IsUint64()) {
      sum += idIt->value.GetUint64();
    }

    auto nestedIt = item.FindMember("nested");
    if (nestedIt != item.MemberEnd() && nestedIt->value.IsObject()) {
      const rapidjson::Value& nested = nestedIt->value;

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
