#include "interface.h"
#include <rapidjson/document.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());
  uint64_t sum = 0;
  for (const auto& item : doc.GetArray()) {
    if (!item["flag"].GetBool()) {
      continue;
    }
    const auto& tag = item["tag"];
    sum += item["id"].GetUint64();
    sum += static_cast<uint64_t>(tag.GetStringLength());
    sum += item["value"].GetUint64();
  }
  return sum;
}