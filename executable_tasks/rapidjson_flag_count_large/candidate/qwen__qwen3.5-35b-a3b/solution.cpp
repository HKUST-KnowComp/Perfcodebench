#include "interface.h"

#include <rapidjson/document.h>
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  uint64_t sum = 0;
  for (auto& item : doc.GetArray()) {
    if (item["flag"].GetBool()) {
      sum += item["id"].GetUint64();
      sum += item["value"].GetUint64();
      sum += item["name"].GetStringLength();
    }
  }
  return sum;
}