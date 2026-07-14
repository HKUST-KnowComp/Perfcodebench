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
  for (auto it = doc.Begin(), end = doc.End(); it != end; ++it) {
    if (!it->IsObject()) continue;

    const auto idIt = it->FindMember("id");
    if (idIt != it->MemberEnd() && idIt->value.IsUint64()) {
      sum += idIt->value.GetUint64();
    }

    const auto nestedIt = it->FindMember("nested");
    if (nestedIt == it->MemberEnd() || !nestedIt->value.IsObject()) continue;

    const auto& nested = nestedIt->value;
    const auto aIt = nested.FindMember("a");
    if (aIt != nested.MemberEnd() && aIt->value.IsUint64()) {
      sum += aIt->value.GetUint64();
    }

    const auto bIt = nested.FindMember("b");
    if (bIt != nested.MemberEnd() && bIt->value.IsUint64()) {
      sum += bIt->value.GetUint64();
    }
  }

  return sum;
}
