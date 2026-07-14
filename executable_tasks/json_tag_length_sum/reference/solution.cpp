#include "interface.h"

#include "simdjson.h"

#include <string_view>

uint64_t compute_checksum(const std::string& json_input) {
  simdjson::dom::parser parser;
  auto doc = parser.parse(json_input);
  uint64_t sum = 0;
  for (auto item : simdjson::dom::array(doc)) {
    if (!bool(item["flag"])) {
      continue;
    }
    const std::string_view tag = std::string_view(item["tag"]);
    sum += uint64_t(item["id"]);
    sum += static_cast<uint64_t>(tag.size());
    sum += uint64_t(item["value"]);
  }
  return sum;
}
