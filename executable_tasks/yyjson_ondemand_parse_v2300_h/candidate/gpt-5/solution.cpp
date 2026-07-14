#include "interface.h"

#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;

  yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
  if (!doc) return 0;

  yyjson_val* root = yyjson_doc_get_root(doc);
  if (!root || !yyjson_is_arr(root)) {
    yyjson_doc_free(doc);
    return 0;
  }

  size_t idx, max;
  yyjson_val* obj;
  yyjson_arr_foreach(root, idx, max, obj) {
    if (!obj || !yyjson_is_obj(obj)) continue;

    yyjson_val* idv = yyjson_obj_get(obj, "id");
    if (idv && yyjson_is_num(idv)) {
      sum += yyjson_get_uint(idv);
    }

    yyjson_val* nested = yyjson_obj_get(obj, "nested");
    if (nested && yyjson_is_obj(nested)) {
      yyjson_val* av = yyjson_obj_get(nested, "a");
      if (av && yyjson_is_num(av)) sum += yyjson_get_uint(av);
      yyjson_val* bv = yyjson_obj_get(nested, "b");
      if (bv && yyjson_is_num(bv)) sum += yyjson_get_uint(bv);
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
