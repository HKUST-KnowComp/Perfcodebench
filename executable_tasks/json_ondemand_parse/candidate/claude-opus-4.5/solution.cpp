#include "interface.h"

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(json_input);
  simdjson::ondemand::document doc = parser.iterate(padded);
  
  uint64_t sum = 0;
  for (simdjson::ondemand::object item : doc.get_array()) {
    sum += uint64_t(item["id"]);
    simdjson::ondemand::object nested = item["nested"].get_object();
    sum += uint64_t(nested["a"]);
    sum += uint64_t(nested["b"]);
  }
  return sum;
}