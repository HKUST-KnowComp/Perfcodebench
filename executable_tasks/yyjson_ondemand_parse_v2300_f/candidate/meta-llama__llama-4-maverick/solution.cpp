#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read_opts((const char*)json_input.data(), json_input.size(), 0, NULL, NULL);
  if (!doc) return 0;
  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;
  yyjson_val *arr, *item, *id, *nested, *a, *b;
  size_t idx, max;
  arr = root;
  if (yyjson_is_arr(arr)) {
    yyjson_arr_iter_init(arr, &idx, &max, &item);
    while (idx < max) {
      id = yyjson_obj_get(item, "id");
      nested = yyjson_obj_get(item, "nested");
      if (id && yyjson_is_uint(id) && nested && yyjson_is_obj(nested)) {
        a = yyjson_obj_get(nested, "a");
        b = yyjson_obj_get(nested, "b");
        if (a && yyjson_is_uint(a) && b && yyjson_is_uint(b)) {
          sum += yyjson_get_uint(id);
          sum += yyjson_get_uint(a);
          sum += yyjson_get_uint(b);
        }
      }
      yyjson_arr_iter_next(&idx, &max, &item);
    }
  }
  yyjson_doc_free(doc);
  return sum;
}