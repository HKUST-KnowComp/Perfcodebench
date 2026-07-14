#include "interface.h"

#include <cstdint>
#include <string>

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::ondemand::parser parser;
  uint64_t sum = 0;

  auto doc_result = parser.iterate(json_input);
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

    uint64_t id = 0;
    uint64_t x = 0;
    uint64_t v1 = 0;

    auto id_res = obj["id"].get_uint64();
    if (id_res.error()) {
      return 0;
    }
    id = id_res.value();

    auto x_res = obj["x"].get_uint64();
    if (x_res.error()) {
      return 0;
    }
    x = x_res.value();

    auto vals_res = obj["vals"].get_array();
    if (vals_res.error()) {
      return 0;
    }
    simdjson::ondemand::array vals = vals_res.value();

    int idx = 0;
    for (auto vv : vals) {
      if (idx == 1) {
        auto v_res = vv.get_uint64();
        if (v_res.error()) {
          return 0;
        }
        v1 = v_res.value();
        break;
      }
      ++idx;
    }

    sum += id + x + v1;
  }

  return sum;
}
