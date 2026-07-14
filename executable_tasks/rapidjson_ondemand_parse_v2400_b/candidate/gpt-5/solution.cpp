#include "interface.h"

#include <rapidjson/document.h>
#include <cstdint>
#include <string>

static inline uint64_t extract_u64(const rapidjson::Value& v) {
  if (v.IsUint64()) return v.GetUint64();
  if (v.IsUint()) return v.GetUint();
  if (v.IsInt64()) {
    int64_t x = v.GetInt64();
    return x >= 0 ? static_cast<uint64_t>(x) : 0u;
  }
  if (v.IsDouble()) {
    double d = v.GetDouble();
    if (d <= 0) return 0u;
    return static_cast<uint64_t>(d);
  }
  return 0u;
}

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document d;
  d.Parse(json_input.c_str());
  if (d.HasParseError() || !d.IsArray()) {
    return 0;
  }

  uint64_t sum = 0;
  const auto id_key = rapidjson::StringRef("id");
  const auto nested_key = rapidjson::StringRef("nested");
  const auto a_key = rapidjson::StringRef("a");
  const auto b_key = rapidjson::StringRef("b");

  const auto arr = d.GetArray();
  for (auto it = arr.Begin(); it != arr.End(); ++it) {
    const rapidjson::Value& obj = *it;
    if (!obj.IsObject()) continue;

    // id
    auto mid = obj.FindMember(id_key);
    if (mid != obj.MemberEnd()) {
      sum += extract_u64(mid->value);
    }

    // nested.a and nested.b
    auto mnested = obj.FindMember(nested_key);
    if (mnested != obj.MemberEnd() && mnested->value.IsObject()) {
      const rapidjson::Value& nested = mnested->value;
      auto ma = nested.FindMember(a_key);
      if (ma != nested.MemberEnd()) sum += extract_u64(ma->value);
      auto mb = nested.FindMember(b_key);
      if (mb != nested.MemberEnd()) sum += extract_u64(mb->value);
    }
  }

  return sum;
}
