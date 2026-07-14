#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document d;
  d.Parse(json_input.c_str());
  uint64_t sum = 0;
  if (d.IsArray()) {
    for (rapidjson::SizeType i = 0; i < d.Size(); i++) {
      sum += d[i]["id"].GetUint64();
      sum += d[i]["nested"]["a"].GetUint64();
      sum += d[i]["nested"]["b"].GetUint64();
    }
  }
  return sum;
}