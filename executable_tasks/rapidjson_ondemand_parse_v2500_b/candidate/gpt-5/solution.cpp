#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), static_cast<rapidjson::SizeType>(json_input.size()));

  uint64_t sum = 0;
  if (!doc.IsArray()) {
    return 0;
  }

  const auto arr = doc.GetArray();
  const rapidjson::SizeType n = arr.Size();
  for (rapidjson::SizeType i = 0; i < n; ++i) {
    const rapidjson::Value& v = arr[i];
    const auto obj = v.GetObject();
    sum += obj["id"].GetUint64();
    const auto nobj = obj["nested"].GetObject();
    sum += nobj["a"].GetUint64();
    sum += nobj["b"].GetUint64();
  }

  return sum;
}
