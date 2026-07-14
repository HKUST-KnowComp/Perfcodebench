#include "interface.h"
#include <simdjson.h>

std::string minify_json(const std::string& json_input) {
  simdjson::padded_string padded_input(json_input);
  std::string minified_output;
  simdjson::minify(padded_input, minified_output);
  return minified_output;
}