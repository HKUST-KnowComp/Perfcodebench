#include "interface.h"

#include <cstdint>
#include <string>

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());

  uint64_t sum = 0;
  if (!doc.IsArray()) return 0;

  const auto arr = doc.GetArray();
  for (auto it = arr.Begin(); it != arr.End(); ++it) {
    const rapidjson::Value& item = *it;
    if (!item.IsObject()) continue;

    auto idIt = item.FindMember("id");
    if (idIt != item.MemberEnd()) {
      const rapidjson::Value& v = idIt->value;
      if (v.IsUint64()) sum += v.GetUint64();
      else if (v.IsUint()) sum += v.GetUint();
      else if (v.IsInt64()) sum += static_cast<uint64_t>(v.GetInt64());
      else if (v.IsInt()) sum += static_cast<uint64_t>(v.GetInt());
    }

    auto xIt = item.FindMember("x");
    if (xIt != item.MemberEnd()) {
      const rapidjson::Value& v = xIt->value;
      if (v.IsUint64()) sum += v.GetUint64();
      else if (v.IsUint()) sum += v.GetUint();
      else if (v.IsInt64()) sum += static_cast<uint64_t>(v.GetInt64());
      else if (v.IsInt()) sum += static_cast<uint64_t>(v.GetInt());
    }

    auto valsIt = item.FindMember("vals");
    if (valsIt != item.MemberEnd()) {
      const rapidjson::Value& vals = valsIt->value;
      if (vals.IsArray() && vals.Size() > 1) {
        const rapidjson::Value& v = vals[1];
        if (v.IsUint64()) sum += v.GetUint64();
        else if (v.IsUint()) sum += v.GetUint();
        else if (v.IsInt64()) sum += static_cast<uint64_t>(v.GetInt64());
        else if (v.IsInt()) sum += static_cast<uint64_t>(v.GetInt());
      }
    }
  }
  return sum;
}
