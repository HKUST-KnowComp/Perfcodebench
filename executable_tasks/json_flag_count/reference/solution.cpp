#include "interface.h"

#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::dom::parser parser;
  auto doc = parser.parse(json_input);
  uint64_t sum = 0;
  for (auto item : simdjson::dom::array(doc)) {
    if (bool(item["flag"])) {
      sum += uint64_t(item["id"]);
    }
  }
  return sum;
}
