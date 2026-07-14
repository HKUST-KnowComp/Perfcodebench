#include "interface.h"

#include <rapidjson/document.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  uint64_t sum = 0;
  for (auto& item : doc.GetArray()) {
    if (!item["flag"].GetBool()) {
      continue;
    }
    const auto& tag = item["tag"];
    uint64_t id = item["id"].GetUint64();
    uint64_t value = item["value"].GetUint64();
    sum += id;
    sum += tag.GetStringLength();
    sum += value;
  }
  return sum;
}