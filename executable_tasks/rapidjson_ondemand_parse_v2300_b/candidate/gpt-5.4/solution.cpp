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
  for (const auto& item : doc.GetArray()) {
    if (!item.IsObject()) continue;

    auto id_it = item.FindMember("id");
    if (id_it != item.MemberEnd() && id_it->value.IsUint64()) {
      sum += id_it->value.GetUint64();
    }

    auto nested_it = item.FindMember("nested");
    if (nested_it == item.MemberEnd() || !nested_it->value.IsObject()) {
      continue;
    }

    const auto& nested = nested_it->value;

    auto a_it = nested.FindMember("a");
    if (a_it != nested.MemberEnd() && a_it->value.IsUint64()) {
      sum += a_it->value.GetUint64();
    }

    auto b_it = nested.FindMember("b");
    if (b_it != nested.MemberEnd() && b_it->value.IsUint64()) {
      sum += b_it->value.GetUint64();
    }
  }

  return sum;
}
