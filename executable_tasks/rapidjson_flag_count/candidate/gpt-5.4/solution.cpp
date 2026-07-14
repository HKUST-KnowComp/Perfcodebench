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
  for (const auto& item : doc.GetArray()) {
    if (!item.IsObject()) {
      continue;
    }

    const auto flag_it = item.FindMember("flag");
    if (flag_it == item.MemberEnd() || !flag_it->value.IsBool() || !flag_it->value.GetBool()) {
      continue;
    }

    const auto id_it = item.FindMember("id");
    const auto value_it = item.FindMember("value");
    const auto name_it = item.FindMember("name");

    if (id_it == item.MemberEnd() || value_it == item.MemberEnd() || name_it == item.MemberEnd()) {
      continue;
    }
    if (!id_it->value.IsUint64() || !value_it->value.IsUint64() || !name_it->value.IsString()) {
      continue;
    }

    sum += id_it->value.GetUint64();
    sum += value_it->value.GetUint64();
    sum += static_cast<uint64_t>(name_it->value.GetStringLength());
  }

  return sum;
}
