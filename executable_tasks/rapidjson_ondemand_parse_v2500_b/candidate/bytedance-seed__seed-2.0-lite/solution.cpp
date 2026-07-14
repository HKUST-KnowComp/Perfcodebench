#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());

  uint64_t sum = 0;
  const rapidjson::Value::Array& arr = doc.GetArray();
  for (const rapidjson::Value& item : arr) {
    sum += item["id"].GetUint64();
    const rapidjson::Value& nested = item["nested"];
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }
  return sum;
}