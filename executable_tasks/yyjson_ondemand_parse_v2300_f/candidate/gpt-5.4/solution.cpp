#include "interface.h"

#include <cstdint>
#include <string>

#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_read_err err;
  yyjson_doc* doc = yyjson_read_opts(
      json_input.data(),
      json_input.size(),
      YYJSON_READ_NOFLAG,
      nullptr,
      &err);
  if (!doc) return 0;

  uint64_t sum = 0;
  yyjson_val* root = yyjson_doc_get_root(doc);
  if (root && yyjson_is_arr(root)) {
    size_t idx, max;
    yyjson_val* item;
    yyjson_arr_foreach(root, idx, max, item) {
      if (!yyjson_is_obj(item)) continue;

      yyjson_val* idv = yyjson_obj_get(item, "id");
      if (idv) sum += yyjson_get_uint(idv);

      yyjson_val* nested = yyjson_obj_get(item, "nested");
      if (nested && yyjson_is_obj(nested)) {
        yyjson_val* av = yyjson_obj_get(nested, "a");
        if (av) sum += yyjson_get_uint(av);
        yyjson_val* bv = yyjson_obj_get(nested, "b");
        if (bv) sum += yyjson_get_uint(bv);
      }
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
