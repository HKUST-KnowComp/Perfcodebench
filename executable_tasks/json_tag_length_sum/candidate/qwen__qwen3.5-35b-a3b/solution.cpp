#include "interface.h"
#include <simdjson.h>
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::dom::parser parser;
  simdjson::dom::element doc = parser.parse(json_input);
  uint64_t sum = 0;
  for (auto& item : doc.get_array()) {
    bool flag = item["flag"].get_bool();
    if (!flag) continue;
    auto tag = item["tag"].get_string();
    sum += item["id"].get_uint64();
    sum += item["value"].get_uint64();
    sum += tag.size();
  }
  return sum;
}