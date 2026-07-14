#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), NULL);
  if (!doc) return 0;
  yyjson_iter* iter = yyjson_iter_new(doc);
  uint64_t sum = 0;
  while (yyjson_iter_next(iter)) {
    if (yyjson_iter_is_object(iter)) {
      yyjson_iter* id_iter = yyjson_iter_get(iter, "id");
      if (id_iter) sum += yyjson_iter_get_uint64(id_iter);
      yyjson_iter* nested_iter = yyjson_iter_get(iter, "nested");
      if (nested_iter && yyjson_iter_is_object(nested_iter)) {
        yyjson_iter* a_iter = yyjson_iter_get(nested_iter, "a");
        if (a_iter) sum += yyjson_iter_get_uint64(a_iter);
        yyjson_iter* b_iter = yyjson_iter_get(nested_iter, "b");
        if (b_iter) sum += yyjson_iter_get_uint64(b_iter);
      }
    }
  }
  yyjson_iter_free(iter);
  yyjson_doc_free(doc);
  return sum;
}