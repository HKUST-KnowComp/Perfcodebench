#include "interface.h"
#include <rapidjson/document.h>

static inline uint64_t read_u64(const rapidjson::Value& v) {
  if (v.IsUint64()) return v.GetUint64();
  if (v.IsInt64()) return static_cast<uint64_t>(v.GetInt64());
  if (v.IsUint()) return static_cast<uint64_t>(v.GetUint());
  if (v.IsInt()) return static_cast<uint64_t>(v.GetInt());
  if (v.IsDouble()) return static_cast<uint64_t>(v.GetDouble());
  return 0;
}

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;
  if (json_input.empty()) return sum;

  rapidjson::Document d;
  d.Parse(json_input.c_str());
  if (d.HasParseError() || !d.IsArray()) return sum;

  const auto arr = d.GetArray();
  for (auto it = arr.Begin(); it != arr.End(); ++it) {
    const rapidjson::Value& item = *it;
    if (!item.IsObject()) continue;

    auto id = item.FindMember("id");
    if (id != item.MemberEnd()) sum += read_u64(id->value);

    auto nested = item.FindMember("nested");
    if (nested != item.MemberEnd() && nested->value.IsObject()) {
      const rapidjson::Value& nest = nested->value;

      auto a = nest.FindMember("a");
      if (a != nest.MemberEnd()) sum += read_u64(a->value);

      auto b = nest.FindMember("b");
      if (b != nest.MemberEnd()) sum += read_u64(b->value);
    }
  }
  return sum;
}
