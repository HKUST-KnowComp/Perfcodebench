#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  
  if (doc.HasParseError() || !doc.IsArray()) {
    return 0;
  }
  
  uint64_t sum = 0;
  for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
    const auto& item = doc[i];
    if (item.IsObject()) {
      if (item.HasMember("id") && item["id"].IsUint64()) {
        sum += item["id"].GetUint64();
      }
      if (item.HasMember("nested") && item["nested"].IsObject()) {
        const auto& nested = item["nested"];
        if (nested.HasMember("a") && nested["a"].IsUint64()) {
          sum += nested["a"].GetUint64();
        }
        if (nested.HasMember("b") && nested["b"].IsUint64()) {
          sum += nested["b"].GetUint64();
        }
      }
    }
  }
  return sum;
}