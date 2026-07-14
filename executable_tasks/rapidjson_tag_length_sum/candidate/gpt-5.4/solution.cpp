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

  for (auto it = doc.Begin(); it != doc.End(); ++it) {
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
    if (!tagIt->value.IsString()) {
      continue;
    }

    uint64_t id = 0;
    if (idIt->value.IsUint64()) {
      id = idIt->value.GetUint64();
    } else if (idIt->value.IsUint()) {
      id = idIt->value.GetUint();
    } else {
      continue;
    }

    uint64_t value = 0;
    if (valueIt->value.IsUint64()) {
      value = valueIt->value.GetUint64();
    } else if (valueIt->value.IsUint()) {
      value = valueIt->value.GetUint();
    } else {
      continue;
    }

    sum += id;
    sum += static_cast<uint64_t>(tagIt->value.GetStringLength());
    sum += value;
  }

  return sum;
}
