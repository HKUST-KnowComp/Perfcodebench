#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document d;
  d.Parse<rapidjson::kParseIterativeFlag>(json_input.c_str());

  uint64_t sum = 0;

  if (!d.IsArray()) {
    return 0;
  }

  static const auto kId = rapidjson::StringRef("id");
  static const auto kNested = rapidjson::StringRef("nested");
  static const auto kA = rapidjson::StringRef("a");
  static const auto kB = rapidjson::StringRef("b");

  auto arr = d.GetArray();
  for (auto it = arr.Begin(); it != arr.End(); ++it) {
    const rapidjson::Value& obj = *it;

    const rapidjson::Value& idVal = obj.FindMember(kId)->value;
    sum += idVal.GetUint64();

    const rapidjson::Value& nestedObj = obj.FindMember(kNested)->value;
    const rapidjson::Value& aVal = nestedObj.FindMember(kA)->value;
    sum += aVal.GetUint64();
    const rapidjson::Value& bVal = nestedObj.FindMember(kB)->value;
    sum += bVal.GetUint64();
  }

  return sum;
}
