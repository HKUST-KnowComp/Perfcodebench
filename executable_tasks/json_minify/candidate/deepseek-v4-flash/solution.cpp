#include "interface.h"

#include <simdjson.h>
#include <string>

std::string minify_json(const std::string& json_input) {
  simdjson::padded_string input(json_input);
  return simdjson::minify(input);
}