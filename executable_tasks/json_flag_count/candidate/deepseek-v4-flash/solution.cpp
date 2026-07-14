#include "interface.h"
#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::padded_string padded(json_input);
  simdjson::ondemand::parser parser;
  simdjson::ondemand::document doc = parser.iterate(padded);
  uint64_t sum = 0;
  simdjson::ondemand::array arr = doc.get_array();
  for (auto element : arr) {
    simdjson::ondemand::object obj = element.get_object();
    bool flag = obj["flag"].get_bool().value();
    if (flag) {
      uint64_t id = obj["id"].get_uint64().value();
      sum += id;
    }
  }
  return sum;
}