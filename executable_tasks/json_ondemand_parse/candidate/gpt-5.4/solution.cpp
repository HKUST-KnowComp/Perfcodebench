#include "interface.h"

#include <cstdint>
#include <string>

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded = simdjson::padded_string::copy(json_input);

  uint64_t sum = 0;

  auto doc_result = parser.iterate(padded);
  if (doc_result.error()) {
    return 0;
  }
  simdjson::ondemand::document doc = doc_result.value();

  auto array_result = doc.get_array();
  if (array_result.error()) {
    return 0;
  }
  simdjson::ondemand::array arr = array_result.value();

  for (auto value : arr) {
    auto obj_result = value.get_object();
    if (obj_result.error()) {
      return 0;
    }
    simdjson::ondemand::object obj = obj_result.value();

    auto id_result = obj["id"].get_uint64();
    if (id_result.error()) {
      return 0;
    }
    sum += id_result.value();

    auto nested_val_result = obj["nested"];
    if (nested_val_result.error()) {
      return 0;
    }
    auto nested_obj_result = nested_val_result.value().get_object();
    if (nested_obj_result.error()) {
      return 0;
    }
    simdjson::ondemand::object nested = nested_obj_result.value();

    auto a_result = nested["a"].get_uint64();
    if (a_result.error()) {
      return 0;
    }
    sum += a_result.value();

    auto b_result = nested["b"].get_uint64();
    if (b_result.error()) {
      return 0;
    }
    sum += b_result.value();
  }

  return sum;
}
