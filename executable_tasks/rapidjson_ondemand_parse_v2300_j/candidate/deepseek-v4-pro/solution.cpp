#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  uint64_t sum = 0;
  if (doc.IsArray()) {
    const auto &arr = doc.GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
      const auto &item = arr[i];
      sum += item["id"].GetUint64();
      const auto &nested = item["nested"];
      sum += nested["a"].GetUint64();
      sum += nested["b"].GetUint64();
    }
  }
  return sum;
}