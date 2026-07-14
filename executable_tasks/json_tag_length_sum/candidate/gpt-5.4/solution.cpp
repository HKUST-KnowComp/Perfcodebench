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

  for (simdjson::ondemand::value item_value : arr) {
    auto obj_result = item_value.get_object();
    if (obj_result.error()) {
      return 0;
    }
    simdjson::ondemand::object obj = obj_result.value();

    bool flag = false;
    uint64_t id = 0;
    uint64_t value = 0;
    uint64_t tag_len = 0;

    for (auto field_result : obj) {
      if (field_result.error()) {
        return 0;
      }
      simdjson::ondemand::field field = field_result.value();

      auto key_result = field.unescaped_key();
      if (key_result.error()) {
        return 0;
      }
      std::string_view key = key_result.value();

      if (key == "flag") {
        auto r = field.value().get_bool();
        if (r.error()) {
          return 0;
        }
        flag = r.value();
      } else if (key == "id") {
        auto r = field.value().get_uint64();
        if (r.error()) {
          return 0;
        }
        id = r.value();
      } else if (key == "value") {
        auto r = field.value().get_uint64();
        if (r.error()) {
          return 0;
        }
        value = r.value();
      } else if (key == "tag") {
        auto r = field.value().get_string();
        if (r.error()) {
          return 0;
        }
        tag_len = static_cast<uint64_t>(r.value().size());
      } else {
        auto skip_err = field.value().skip();
        if (skip_err) {
          return 0;
        }
      }
    }

    if (flag) {
      sum += id + value + tag_len;
    }
  }

  return sum;
}
