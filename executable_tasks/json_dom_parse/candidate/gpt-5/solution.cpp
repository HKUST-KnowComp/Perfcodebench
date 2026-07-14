#include "interface.h"

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  static thread_local simdjson::ondemand::parser parser;

  uint64_t sum = 0;

  // Parse the JSON input as an on-demand document
  simdjson::ondemand::document doc = parser.iterate(json_input).value_unsafe();
  simdjson::ondemand::array arr = doc.get_array().value_unsafe();

  for (simdjson::ondemand::value elem : arr) {
    simdjson::ondemand::object obj = elem.get_object().value_unsafe();

    uint64_t id = obj.find_field_unordered("id").get_uint64().value_unsafe();
    uint64_t x  = obj.find_field_unordered("x").get_uint64().value_unsafe();

    simdjson::ondemand::array vals = obj.find_field_unordered("vals").get_array().value_unsafe();
    uint64_t v1 = vals.at(1).get_uint64().value_unsafe();

    sum += id + x + v1;
  }

  return sum;
}
