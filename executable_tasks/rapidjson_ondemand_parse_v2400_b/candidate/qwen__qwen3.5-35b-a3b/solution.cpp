#include "interface.h"

#include <rapidjson/document.h>
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.c_str());
  uint64_t sum = 0;
  for (auto& item : doc.GetArray()) {
    auto id = item.FindMember("id");
    sum += id->value.GetUint64();
    auto nested = item.FindMember("nested");
    auto a = nested->value.FindMember("a");
    sum += a->value.GetUint64();
    auto b = nested->value.FindMember("b");
    sum += b->value.GetUint64();
  }
  return sum;
}