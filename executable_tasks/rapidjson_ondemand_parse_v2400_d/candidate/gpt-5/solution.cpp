#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse<rapidjson::kParseStopWhenDoneFlag>(json_input.c_str());

  uint64_t sum = 0;
  if (!doc.IsArray()) {
    return sum;
  }

  auto arr = doc.GetArray();
  const rapidjson::SizeType n = arr.Size();
  for (rapidjson::SizeType i = 0; i < n; ++i) {
    const rapidjson::Value& item = arr[i];
    sum += item[rapidjson::StringRef("id")].GetUint64();
    const rapidjson::Value& nested = item[rapidjson::StringRef("nested")];
    sum += nested[rapidjson::StringRef("a")].GetUint64();
    sum += nested[rapidjson::StringRef("b")].GetUint64();
  }

  return sum;
}
