#include "interface.h"

#include <cstdint>
#include <string>

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());

  uint64_t sum = 0;
  if (!doc.IsArray()) return 0;

  for (auto it = doc.Begin(), end = doc.End(); it != end; ++it) {
    const rapidjson::Value& item = *it;
    const rapidjson::Value& nested = item["nested"];
    sum += item["id"].GetUint64();
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }
  return sum;
}
