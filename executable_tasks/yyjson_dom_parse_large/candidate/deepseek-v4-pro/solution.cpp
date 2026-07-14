#include "interface.h"

#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
  if (!doc) {
    return 0; // or handle error appropriately
  }

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

    yyjson_val *id_val = yyjson_obj_get(item, "id");
    yyjson_val *x_val  = yyjson_obj_get(item, "x");
    yyjson_val *vals   = yyjson_obj_get(item, "vals");
    if (!vals) continue;
    yyjson_val *val1   = yyjson_arr_get(vals, 1);

    uint64_t id = id_val ? yyjson_get_uint(id_val) : 0;
    uint64_t x  = x_val  ? yyjson_get_uint(x_val)  : 0;
    uint64_t v1 = val1   ? yyjson_get_uint(val1)   : 0;

    sum += id + x + v1;
  }

  yyjson_doc_free(doc);
  return sum;
}