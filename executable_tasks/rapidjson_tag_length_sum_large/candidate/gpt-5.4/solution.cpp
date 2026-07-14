#include "interface.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());

  if (doc.HasParseError() || !doc.IsArray()) {
    return 0;
  }

  uint64_t sum = 0;
  for (auto it = doc.Begin(), end = doc.End(); it != end; ++it) {
    const rapidjson::Value& item = *it;
    if (!item.IsObject()) {
      continue;
    }

    auto flagIt = item.FindMember("flag");
    if (flagIt == item.MemberEnd() || !flagIt->value.IsBool() || !flagIt->value.GetBool()) {
      continue;
    }

    auto idIt = item.FindMember("id");
    auto tagIt = item.FindMember("tag");
    auto valueIt = item.FindMember("value");

    if (idIt == item.MemberEnd() || valueIt == item.MemberEnd() || tagIt == item.MemberEnd()) {
      continue;
    }
    if (!idIt->value.IsUint64() || !valueIt->value.IsUint64() || !tagIt->value.IsString()) {
      continue;
    }

    sum += idIt->value.GetUint64();
    sum += static_cast<uint64_t>(tagIt->value.GetStringLength());
    sum += valueIt->value.GetUint64();
  }

  return sum;
}
