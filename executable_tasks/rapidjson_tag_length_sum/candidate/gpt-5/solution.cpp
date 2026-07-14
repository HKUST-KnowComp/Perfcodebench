#include "interface.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  // Parse using the input buffer without modifying it.
  if (doc.Parse(json_input.data(), json_input.size()).HasParseError()) {
    return 0;  // Assuming valid input per harness; return 0 on parse error.
  }

  if (!doc.IsArray()) {
    return 0;  // Defensive: expect a top-level array.
  }

  uint64_t sum = 0;

  // Predefined keys and lengths to avoid repeated strlen
  static constexpr const char kFlag[] = "flag";
  static constexpr const char kTag[] = "tag";
  static constexpr const char kId[] = "id";
  static constexpr const char kValue[] = "value";

  const auto arr = doc.GetArray();
  for (const auto& v : arr) {
    if (!v.IsObject()) {
      continue;
    }
    const auto obj = v.GetObject();

    const auto end = obj.MemberEnd();

    const auto itFlag = obj.FindMember(rapidjson::StringRef(kFlag, sizeof(kFlag) - 1));
    if (itFlag == end || !itFlag->value.IsBool() || !itFlag->value.GetBool()) {
      continue;
    }

    const auto itTag = obj.FindMember(rapidjson::StringRef(kTag, sizeof(kTag) - 1));
    const auto itId = obj.FindMember(rapidjson::StringRef(kId, sizeof(kId) - 1));
    const auto itVal = obj.FindMember(rapidjson::StringRef(kValue, sizeof(kValue) - 1));

    if (itTag == end || !itTag->value.IsString() ||
        itId == end || !itId->value.IsUint64() ||
        itVal == end || !itVal->value.IsUint64()) {
      continue;  // Defensive: skip malformed entries.
    }

    sum += itId->value.GetUint64();
    sum += static_cast<uint64_t>(itTag->value.GetStringLength());
    sum += itVal->value.GetUint64();
  }

  return sum;
}
