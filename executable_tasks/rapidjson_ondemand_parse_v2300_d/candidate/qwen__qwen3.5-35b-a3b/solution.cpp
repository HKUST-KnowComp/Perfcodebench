#pragma once

#include <cstdint>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  
  uint64_t sum = 0;
  for (const auto& item : doc.GetArray()) {
    const auto& id = item["id"];
    const auto& nested = item["nested"];
    sum += id.GetUint64();
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }
  return sum;
}