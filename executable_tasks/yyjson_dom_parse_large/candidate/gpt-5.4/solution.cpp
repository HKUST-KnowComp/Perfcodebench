#include "interface.h"

#include <cstdint>
#include <string>

#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_read_err err;
  yyjson_doc *doc = yyjson_read_opts(
      json_input.data(),
      json_input.size(),
      YYJSON_READ_NOFLAG,
      nullptr,
      &err);

  if (!doc) return 0;

  yyjson_val *root = yyjson_doc_get_root(doc);
  if (!root || !yyjson_is_arr(root)) {
    yyjson_doc_free(doc);
    return 0;
  }

  uint64_t sum = 0;

  size_t idx, max;
  yyjson_val *item;
  yyjson_arr_foreach(root, idx, max, item) {
    if (!yyjson_is_obj(item)) continue;

    yyjson_val *id = yyjson_obj_get(item, "id");
    yyjson_val *x = yyjson_obj_get(item, "x");
    yyjson_val *vals = yyjson_obj_get(item, "vals");

    if (id && yyjson_is_num(id)) sum += yyjson_get_uint(id);
    if (x && yyjson_is_num(x)) sum += yyjson_get_uint(x);

    if (vals && yyjson_is_arr(vals)) {
      yyjson_val *v1 = yyjson_arr_get(vals, 1);
      if (v1 && yyjson_is_num(v1)) sum += yyjson_get_uint(v1);
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
