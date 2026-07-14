#include "interface.h"

#include <string>
#include <yyjson.h>

std::string minify_json(const std::string& json_input) {
  yyjson_read_err err;
  yyjson_doc* doc = yyjson_read_opts(
      json_input.data(),
      json_input.size(),
      YYJSON_READ_NOFLAG,
      nullptr,
      &err);

  if (!doc) {
    return std::string();
  }

  yyjson_mut_doc* mut_doc = yyjson_doc_mut_copy(doc, nullptr);
  yyjson_doc_free(doc);
  if (!mut_doc) {
    return std::string();
  }

  size_t len = 0;
  char* out = yyjson_mut_write_opts(
      mut_doc,
      YYJSON_WRITE_NOFLAG,
      nullptr,
      &len,
      nullptr);

  yyjson_mut_doc_free(mut_doc);

  if (!out) {
    return std::string();
  }

  std::string result(out, len);
  free(out);
  return result;
}
