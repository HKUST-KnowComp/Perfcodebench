#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
  static thread_local simdjson::ondemand::parser parser;
  uint64_t sum = 0;

  simdjson::ondemand::document doc = parser.iterate(json_input);
  simdjson::ondemand::array arr = doc.get_array().value_unsafe();
  for (simdjson::ondemand::value elem : arr) {
    simdjson::ondemand::object obj = elem.get_object().value_unsafe();

    uint64_t id = obj.find_field_unordered("id").get_uint64().value_unsafe();

    simdjson::ondemand::object nested = obj.find_field_unordered("nested").get_object().value_unsafe();
    uint64_t a = nested.find_field_unordered("a").get_uint64().value_unsafe();
    uint64_t b = nested.find_field_unordered("b").get_uint64().value_unsafe();

    sum += id + a + b;
  }

  return sum;
}
