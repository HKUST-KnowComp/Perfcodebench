#include "interface.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  // Parse using the provided buffer and length to avoid implicit strlen.
  doc.Parse(json_input.data(), json_input.size());

  uint64_t sum = 0;

  if (!doc.IsArray()) {
    return 0;
  }

  const auto kFlag = rapidjson::StringRef("flag");
  const auto kId = rapidjson::StringRef("id");
  const auto kTag = rapidjson::StringRef("tag");
  const auto kValue = rapidjson::StringRef("value");

  for (auto& v : doc.GetArray()) {
    if (!v.IsObject()) {
      continue;
    }
    const rapidjson::Value& obj = v;

    const auto fit = obj.FindMember(kFlag);
    if (fit == obj.MemberEnd() || !fit->value.IsBool() || !fit->value.GetBool()) {
      continue;
    }

    // id
    uint64_t id = 0;
    if (const auto it = obj.FindMember(kId); it != obj.MemberEnd()) {
      const auto& val = it->value;
      if (val.IsUint64()) {
        id = val.GetUint64();
      } else if (val.IsInt64()) {
        const int64_t t = val.GetInt64();
        if (t >= 0) id = static_cast<uint64_t>(t);
      } else if (val.IsUint()) {
        id = val.GetUint();
      } else if (val.IsInt()) {
        const int32_t t = val.GetInt();
        if (t >= 0) id = static_cast<uint64_t>(t);
      }
    }
    sum += id;

    // tag length
    size_t tag_len = 0;
    if (const auto it = obj.FindMember(kTag); it != obj.MemberEnd() && it->value.IsString()) {
      tag_len = it->value.GetStringLength();
    }
    sum += static_cast<uint64_t>(tag_len);

    // value
    uint64_t value = 0;
    if (const auto it = obj.FindMember(kValue); it != obj.MemberEnd()) {
      const auto& val = it->value;
      if (val.IsUint64()) {
        value = val.GetUint64();
      } else if (val.IsInt64()) {
        const int64_t t = val.GetInt64();
        if (t >= 0) value = static_cast<uint64_t>(t);
      } else if (val.IsUint()) {
        value = val.GetUint();
      } else if (val.IsInt()) {
        const int32_t t = val.GetInt();
        if (t >= 0) value = static_cast<uint64_t>(t);
      }
    }
    sum += value;
  }

  return sum;
}
