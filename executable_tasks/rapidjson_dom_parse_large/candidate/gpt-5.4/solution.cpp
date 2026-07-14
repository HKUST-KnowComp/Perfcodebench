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
  const auto& arr = doc.GetArray();
  for (auto it = arr.Begin(), end = arr.End(); it != end; ++it) {
    const rapidjson::Value& item = *it;

    const auto idIt = item.FindMember("id");
    if (idIt != item.MemberEnd() && idIt->value.IsUint64()) {
      sum += idIt->value.GetUint64();
    }

    const auto xIt = item.FindMember("x");
    if (xIt != item.MemberEnd() && xIt->value.IsUint64()) {
      sum += xIt->value.GetUint64();
    }

    const auto valsIt = item.FindMember("vals");
    if (valsIt != item.MemberEnd()) {
      const rapidjson::Value& vals = valsIt->value;
      if (vals.IsArray() && vals.Size() > 1 && vals[1].IsUint64()) {
        sum += vals[1].GetUint64();
      }
    }
  }
  return sum;
}
