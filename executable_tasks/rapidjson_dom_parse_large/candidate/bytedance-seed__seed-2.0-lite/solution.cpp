#include "interface.h"
#include "rapidjson/document.h"

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());
  uint64_t sum = 0;
  const auto& arr = doc.GetArray();
  for (const auto& item : arr) {
    sum += item["id"].GetUint64();
    sum += item["x"].GetUint64();
    sum += item["vals"][1].GetUint64();
  }
  return sum;
}