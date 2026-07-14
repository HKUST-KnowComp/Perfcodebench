#include "interface.h"

#include <cstdint>
#include <string>

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  rapidjson::Document doc;
  doc.Parse(json_input.data(), json_input.size());

  uint64_t sum = 0;

  const auto id_key = rapidjson::StringRef("id");
  const auto nested_key = rapidjson::StringRef("nested");
  const auto a_key = rapidjson::StringRef("a");
  const auto b_key = rapidjson::StringRef("b");

  for (auto it = doc.Begin(); it != doc.End(); ++it) {
    const rapidjson::Value& item = *it;
    const rapidjson::Value& nested = item[nested_key];
    sum += item[id_key].GetUint64();
    sum += nested[a_key].GetUint64();
    sum += nested[b_key].GetUint64();
  }

  return sum;
}
