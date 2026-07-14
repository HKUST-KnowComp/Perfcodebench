#include "interface.h"
#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());
  const auto& arr = doc.GetArray();
  uint64_t sum = 0;
  for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
    const auto& item = arr[i];
    if (!item["flag"].GetBool()) continue;
    sum += item["id"].GetUint64();
    sum += item["value"].GetUint64();
    sum += item["name"].GetStringLength();
  }
  return sum;
}