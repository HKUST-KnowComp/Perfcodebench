#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse<rapidjson::kParseInsituFlag>(const_cast<char*>(json_input.data()));
  
  uint64_t sum = 0;
  const auto& arr = doc.GetArray();
  for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
    const auto& item = arr[i];
    sum += item["id"].GetUint64();
    sum += item["x"].GetUint64();
    sum += item["vals"][1].GetUint64();
  }
  return sum;
}