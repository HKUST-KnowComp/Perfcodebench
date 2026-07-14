#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  // Parse using length-based API and stop-when-done for speed
  doc.Parse<rapidjson::kParseStopWhenDoneFlag>(json_input.data(), json_input.size());

  uint64_t sum = 0;
  if (!doc.IsArray()) {
    return sum;
  }

  const auto arr = doc.GetArray();
  for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
    const rapidjson::Value& item = arr[i];
    sum += item["id"].GetUint64();
    const rapidjson::Value& nested = item["nested"];
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }

  return sum;
}
