#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  // Create mutable copy for in-situ parsing
  std::string mutable_input = json_input;
  doc.ParseInsitu(&mutable_input[0]);
  
  uint64_t sum = 0;
  const auto& array = doc.GetArray();
  for (rapidjson::SizeType i = 0; i < array.Size(); ++i) {
    const auto& item = array[i];
    sum += item["id"].GetUint64();
    const auto& nested = item["nested"];
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }
  return sum;
}