#include "interface.h"
#include <simdjson.h>

std::string minify_json(const std::string& json_input) {
  std::string minified_output;
  simdjson::minify(json_input.data(), json_input.size(), minified_output);
  return minified_output;
}