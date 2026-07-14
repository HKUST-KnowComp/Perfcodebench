#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;
  yyjson_doc* doc = yyjson_parse(json_input.c_str(), NULL);
  if (doc == NULL) {
    return 0;
  }
  yyjson_iter* iter = yyjson_iter_new(doc);
  if (iter == NULL) {
    yyjson_doc_free(doc);
    return 0;
  }
  yyjson_iter_next(iter);
  while (yyjson_iter_has_next(iter)) {
    yyjson_iter_next(iter);
    uint64_t id = yyjson_iter_get_uint64(iter, "id");
    sum += id;
    yyjson_iter* nested_iter = yyjson_iter_get(iter, "nested");
    if (nested_iter != NULL) {
      uint64_t a = yyjson_iter_get_uint64(nested_iter, "a");
      uint64_t b = yyjson_iter_get_uint64(nested_iter, "b");
      sum += a + b;
    }
    yyjson_iter_next(iter);
  }
  yyjson_iter_free(iter);
  yyjson_doc_free(doc);
  return sum;
}