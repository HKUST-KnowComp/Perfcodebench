#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::ondemand::parser parser;
  auto doc = parser.iterate(json_input);
  uint64_t sum = 0;
  for (auto item : doc) {
    sum += item["id"].get_uint64();
    auto nested = item["nested"].get_object();
    sum += nested["a"].get_uint64();
    sum += nested["b"].get_uint64();
  }
  return sum;
}