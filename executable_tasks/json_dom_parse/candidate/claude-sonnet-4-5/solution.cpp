#include "interface.h"

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::dom::parser parser;
  simdjson::dom::element doc = parser.parse(json_input);
  
  uint64_t sum = 0;
  for (simdjson::dom::element item : doc.get_array()) {
    sum += uint64_t(item["id"]);
    sum += uint64_t(item["x"]);
    sum += uint64_t(item["vals"].get_array().at(1));
  }
  return sum;
}