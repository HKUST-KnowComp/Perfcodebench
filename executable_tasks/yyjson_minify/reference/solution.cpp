#include "interface.h"

#include "yyjson.h"

#include <cstdlib>
#include <string>

std::string minify_json(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
  if (doc == nullptr) {
    return {};
  }
  size_t len = 0;
  char* out = yyjson_write(doc, 0, &len);
  std::string result;
  if (out != nullptr) {
    result.assign(out, len);
    std::free(out);
  }
  yyjson_doc_free(doc);
  return result;
}
