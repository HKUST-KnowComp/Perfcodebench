#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
  using namespace simdjson;
  uint64_t sum = 0;
  padded_string padded_json(json_input);
  ondemand::parser parser;
  auto doc = parser.iterate(padded_json);
  
  for (auto element : doc.get_array()) {
    if (element["flag"].get_bool()) {
      sum += element["id"].get_uint64();
    }
  }
  return sum;
}