#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());

  uint64_t sum = 0;
  if (!doc.IsArray()) {
    return sum; // Input is assumed valid in harness; this mirrors safe fallback
  }

  auto arr = doc.GetArray();
  for (auto& v : arr) {
    sum += v["id"].GetUint64();
    const auto& nested = v["nested"];
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }

  return sum;
}
