#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::dom::parser parser;
  simdjson::dom::document doc = parser.parse(json_input);
  uint64_t sum = 0;
  for (simdjson::dom::element item : doc) {
    simdjson::dom::object obj = item.get_object();
    bool flag = obj["flag"].get_bool();
    if (!flag) continue;
    sum += obj["id"].get_uint64();
    sum += static_cast<uint64_t>(obj["tag"].get_string().value().size());
    sum += obj["value"].get_uint64();
  }
  return sum;
}
