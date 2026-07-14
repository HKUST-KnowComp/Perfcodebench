#include "interface.h"
#include <simdjson.h>

std::string minify_json(const std::string& json_input) {
  simdjson::padded_string padded(json_input);
  auto doc = simdjson::parse(padded);
  return simdjson::to_string(doc);
}