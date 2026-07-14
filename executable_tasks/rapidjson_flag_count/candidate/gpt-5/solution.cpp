#include "interface.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  using namespace rapidjson;

  uint64_t sum = 0;
  if (json_input.empty()) {
    return 0;
  }

  // In-situ parse to avoid per-string allocations: RapidJSON will reuse this buffer.
  std::string buffer(json_input);
  Document d;
  d.ParseInsitu(buffer.data());
  if (d.HasParseError() || !d.IsArray()) {
    return 0;
  }

  auto arr = d.GetArray();
  for (auto it = arr.Begin(); it != arr.End(); ++it) {
    const Value& v = *it;
    if (!v.IsObject()) continue;

    const Value& obj = v;

    // flag must be true
    auto flagIt = obj.FindMember("flag");
    if (flagIt == obj.MemberEnd()) continue;
    const Value& fv = flagIt->value;
    if (!fv.IsBool() || !fv.GetBool()) continue;

    uint64_t id = 0;
    auto idIt = obj.FindMember("id");
    if (idIt != obj.MemberEnd()) {
      const Value& iv = idIt->value;
      if (iv.IsUint64())      id = iv.GetUint64();
      else if (iv.IsUint())   id = iv.GetUint();
      else if (iv.IsInt64()) {
        int64_t t = iv.GetInt64();
        if (t > 0) id = static_cast<uint64_t>(t);
      } else if (iv.IsInt()) {
        int t = iv.GetInt();
        if (t > 0) id = static_cast<uint64_t>(t);
      }
    }

    uint64_t value = 0;
    auto valIt = obj.FindMember("value");
    if (valIt != obj.MemberEnd()) {
      const Value& vv = valIt->value;
      if (vv.IsUint64())      value = vv.GetUint64();
      else if (vv.IsUint())   value = vv.GetUint();
      else if (vv.IsInt64()) {
        int64_t t = vv.GetInt64();
        if (t > 0) value = static_cast<uint64_t>(t);
      } else if (vv.IsInt()) {
        int t = vv.GetInt();
        if (t > 0) value = static_cast<uint64_t>(t);
      }
    }

    uint64_t nameLen = 0;
    auto nameIt = obj.FindMember("name");
    if (nameIt != obj.MemberEnd()) {
      const Value& nv = nameIt->value;
      if (nv.IsString()) nameLen = static_cast<uint64_t>(nv.GetStringLength());
    }

    sum += id;
    sum += value;
    sum += nameLen;
  }

  return sum;
}
