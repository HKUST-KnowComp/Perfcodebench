#include "interface.h"

#include <rapidjson/document.h>

uint64_t compute_checksum(const std::string& json_input) {
  // Make a modifiable copy so we can use RapidJSON in-situ parsing, which
  // avoids copying strings for object keys and values.
  std::string buffer = json_input;

  rapidjson::Document doc;
  doc.ParseInsitu(&buffer[0]);

  uint64_t sum = 0;
  for (auto& item : doc.GetArray()) {
    auto obj = item.GetObject();
    sum += obj["id"].GetUint64();
    auto nested = obj["nested"].GetObject();
    sum += nested["a"].GetUint64();
    sum += nested["b"].GetUint64();
  }
  return sum;
}