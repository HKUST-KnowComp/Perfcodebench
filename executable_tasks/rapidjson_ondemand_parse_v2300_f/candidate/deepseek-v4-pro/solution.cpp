#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  // Assume valid JSON as per harness; ignore parse errors in release
  const auto& arr = doc.GetArray();
  uint64_t sum = 0;
  for (const auto& item : arr) {
    sum += item["id"].GetUint64();
    const auto& nested = item["nested"];
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }
  return sum;
}