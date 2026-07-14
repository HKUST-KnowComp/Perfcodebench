#include "interface.h"
#include <simdjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::ondemand::parser parser;
  auto json = parser.iterate(json_input);
  auto array = json.get_array();
  uint64_t sum = 0;
  for (auto& item : array) {
    auto obj = item.get_object();
    auto flag = obj["flag"];
    if (flag.get_bool()) {
      auto id = obj["id"];
      sum += id.get_uint64();
    }
  }
  return sum;
}