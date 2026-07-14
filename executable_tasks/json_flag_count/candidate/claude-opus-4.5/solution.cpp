#include "interface.h"

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(json_input);
  simdjson::ondemand::document doc = parser.iterate(padded);
  
  uint64_t sum = 0;
  for (auto item : doc.get_array()) {
    simdjson::ondemand::object obj = item.get_object();
    bool flag = false;
    uint64_t id = 0;
    for (auto field : obj) {
      std::string_view key = field.unescaped_key();
      if (key == "flag") {
        flag = field.value().get_bool();
      } else if (key == "id") {
        id = field.value().get_uint64();
      }
    }
    if (flag) {
      sum += id;
    }
  }
  return sum;
}