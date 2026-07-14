#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  const char* json_str = json_input.c_str();
  yyjson_doc* doc = yyjson_parse(json_str, NULL);
  if (!doc) {
    return 0;
  }

  uint64_t sum = 0;
  yyjson_iter* iter = yyjson_iter_new(doc);
  if (yyjson_iter_next(iter)) {
    do {
      yyjson_val* id = yyjson_iter_get(iter, "id");
      if (yyjson_is_uint(id)) {
        sum += yyjson_get_uint(id);
      }

      yyjson_val* nested = yyjson_iter_get(iter, "nested");
      if (yyjson_is_obj(nested)) {
        yyjson_val* a = yyjson_get_obj(nested, "a");
        if (yyjson_is_uint(a)) {
          sum += yyjson_get_uint(a);
        }
        yyjson_val* b = yyjson_get_obj(nested, "b");
        if (yyjson_is_uint(b)) {
          sum += yyjson_get_uint(b);
        }
      }
    } while (yyjson_iter_next(iter));
  }

  yyjson_iter_free(iter);
  yyjson_doc_free(doc);
  return sum;
}