#include "interface.h"

#include <cstdint>
#include <string>

#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(json_input);

  uint64_t sum = 0;

  auto doc_result = parser.iterate(padded);
  if (doc_result.error()) {
    return 0;
  }
  simdjson::ondemand::document doc = doc_result.value();

  auto arr_result = doc.get_array();
  if (arr_result.error()) {
    return 0;
  }
  simdjson::ondemand::array arr = arr_result.value();

  for (simdjson::ondemand::value obj_val : arr) {
    auto obj_result = obj_val.get_object();
    if (obj_result.error()) {
      continue;
    }
    simdjson::ondemand::object obj = obj_result.value();

    bool flag = false;
    uint64_t id = 0;

    for (auto field : obj) {
      auto key_result = field.unescaped_key();
      if (key_result.error()) {
        continue;
      }
      std::string_view key = key_result.value();

      if (key == "flag") {
        auto flag_result = field.value().get_bool();
        if (!flag_result.error()) {
          flag = flag_result.value();
        }
      } else if (key == "id") {
        auto id_result = field.value().get_uint64();
        if (!id_result.error()) {
          id = id_result.value();
        }
      }
    }

    if (flag) {
      sum += id;
    }
  }

  return sum;
}
