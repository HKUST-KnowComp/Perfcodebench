#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse<rapidjson::kParseIterativeFlag | rapidjson::kParseStopWhenDoneFlag>(
      json_input.data(), json_input.size());

  if (!doc.IsArray()) {
    return 0;
  }

  uint64_t sum = 0;
  auto arr = doc.GetArray();
  for (auto& v : arr) {
    auto obj = v.GetObject();

    auto idIt = obj.FindMember("id");
    auto nestedIt = obj.FindMember("nested");

    auto nestedObj = nestedIt->value.GetObject();
    auto aIt = nestedObj.FindMember("a");
    auto bIt = nestedObj.FindMember("b");

    sum += idIt->value.GetUint64();
    sum += aIt->value.GetUint64();
    sum += bIt->value.GetUint64();
  }

  return sum;
}
