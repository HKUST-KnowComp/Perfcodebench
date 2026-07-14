#include "interface.h"

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(json_input);
  simdjson::ondemand::document doc = parser.iterate(padded);
  
  uint64_t sum = 0;
  for (auto item : doc.get_array()) {
    bool flag = item["flag"].get_bool();
    uint64_t id = item["id"].get_uint64();
    std::string_view tag = item["tag"].get_string();
    uint64_t value = item["value"].get_uint64();
    
    uint64_t item_sum = id + static_cast<uint64_t>(tag.size()) + value;
    sum += item_sum * static_cast<uint64_t>(flag);
  }
  return sum;
}