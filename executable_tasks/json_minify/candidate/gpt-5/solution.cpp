#include "interface.h"

#include <simdjson.h>
#include <string>

std::string minify_json(const std::string& json_input) {
  if (json_input.empty()) {
    return std::string();
  }
  std::string output(json_input.size(), '\0');
  size_t out_len = simdjson::minify(
      reinterpret_cast<const uint8_t*>(json_input.data()),
      json_input.size(),
      reinterpret_cast<uint8_t*>(&output[0])
  );
  output.resize(out_len);
  return output;
}
