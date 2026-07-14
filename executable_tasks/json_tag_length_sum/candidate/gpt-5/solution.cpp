#include "interface.h"

#include <simdjson.h>
#include <string>
#include <string_view>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
  // Use simdjson on-demand parser for fast JSON processing
  simdjson::ondemand::parser parser;
  // Create a padded copy of the input for safe simdjson parsing
  simdjson::padded_string padded = simdjson::padded_string::copy(json_input);

  uint64_t sum = 0;

  simdjson::ondemand::document doc = parser.iterate(padded);
  simdjson::ondemand::array arr = doc.get_array();
  for (simdjson::ondemand::value element : arr) {
    simdjson::ondemand::object obj = element.get_object();

    bool flag = obj.find_field_unordered("flag").get_bool().value_unsafe();
    if (!flag) {
      continue;
    }

    uint64_t id = obj.find_field_unordered("id").get_uint64().value_unsafe();
    std::string_view tag = obj.find_field_unordered("tag").get_string().value_unsafe();
    uint64_t value = obj.find_field_unordered("value").get_uint64().value_unsafe();

    sum += id;
    sum += static_cast<uint64_t>(tag.size());
    sum += value;
  }

  return sum;
}
