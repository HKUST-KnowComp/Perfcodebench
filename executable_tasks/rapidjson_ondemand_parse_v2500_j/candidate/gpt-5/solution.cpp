#include "interface.h"

#include <cstdint>
#include <string>
#include <rapidjson/document.h>

using namespace rapidjson;

static inline uint64_t to_u64(const Value& v) {
  if (v.IsUint64()) return v.GetUint64();
  if (v.IsUint()) return static_cast<uint64_t>(v.GetUint());
  if (v.IsInt64()) {
    int64_t x = v.GetInt64();
    return x >= 0 ? static_cast<uint64_t>(x) : 0ULL;
  }
  if (v.IsInt()) {
    int x = v.GetInt();
    return x >= 0 ? static_cast<uint64_t>(x) : 0ULL;
  }
  if (v.IsDouble()) {
    double d = v.GetDouble();
    if (d >= 0.0) return static_cast<uint64_t>(d);
  }
  return 0ULL;
}

uint64_t compute_checksum(const std::string& json_input) {
  Document doc;
  doc.Parse(json_input.c_str());
  uint64_t sum = 0;
  if (!doc.IsArray()) return 0;

  const auto idKey = StringRef("id");
  const auto nestedKey = StringRef("nested");
  const auto aKey = StringRef("a");
  const auto bKey = StringRef("b");

  for (auto& item : doc.GetArray()) {
    if (!item.IsObject()) continue;

    if (auto it = item.FindMember(idKey); it != item.MemberEnd()) {
      sum += to_u64(it->value);
    }

    if (auto nit = item.FindMember(nestedKey); nit != item.MemberEnd() && nit->value.IsObject()) {
      const Value& nested = nit->value;
      if (auto ait = nested.FindMember(aKey); ait != nested.MemberEnd()) {
        sum += to_u64(ait->value);
      }
      if (auto bit = nested.FindMember(bKey); bit != nested.MemberEnd()) {
        sum += to_u64(bit->value);
      }
    }
  }

  return sum;
}
