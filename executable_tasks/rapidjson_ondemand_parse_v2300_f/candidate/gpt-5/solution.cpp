#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document d;
  d.Parse(json_input.c_str(), json_input.size());

  uint64_t sum = 0;
  if (!d.IsArray()) {
    return sum;
  }

  auto arr = d.GetArray();
  const rapidjson::SizeType n = arr.Size();
  for (rapidjson::SizeType i = 0; i < n; ++i) {
    rapidjson::Value& item = arr[i];
    sum += item["id"].GetUint64();
    rapidjson::Value& nested = item["nested"];
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }

  return sum;
}
