#include "interface.h"

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  auto error = parser.parse(json_input).get(doc);
  if (error) {
    return 0;
  }
  
  uint64_t sum = 0;
  simdjson::dom::array arr;
  if (doc.get(arr)) {
    return 0;
  }
  
  for (simdjson::dom::element item : arr) {
    simdjson::dom::object obj;
    if (item.get(obj)) {
      continue;
    }
    
    bool flag_val;
    if (obj["flag"].get(flag_val) == simdjson::SUCCESS && flag_val) {
      uint64_t id_val;
      if (obj["id"].get(id_val) == simdjson::SUCCESS) {
        sum += id_val;
      }
    }
  }
  
  return sum;
}