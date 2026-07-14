#include "interface.h"
#include <rapidjson/document.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  uint64_t sum = 0;
  const auto& arr = doc.GetArray();
  for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
    const auto& item = arr[i];
    if (!item["flag"].GetBool()) continue;
    sum += item["id"].GetUint64();
    sum += static_cast<uint64_t>(item["tag"].GetStringLength());
    sum += item["value"].GetUint64();
  }
  return sum;
}