#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read_opts((const char*)json_input.data(), json_input.size(), 0, NULL, NULL);
  if (!doc) return 0;
  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;
  yyjson_val *arr, *item;
  yyjson_val *id, *nested, *a, *b;
  arr = yyjson_get_array(root);
  size_t len = yyjson_arr_size(arr);
  for (size_t i = 0; i < len; i++) {
    item = yyjson_arr_get(arr, i);
    id = yyjson_obj_get(item, "id");
    nested = yyjson_obj_get(item, "nested");
    a = yyjson_obj_get(nested, "a");
    b = yyjson_obj_get(nested, "b");
    if (id && yyjson_is_uint(id)) sum += yyjson_get_uint(id);
    if (a && yyjson_is_uint(a)) sum += yyjson_get_uint(a);
    if (b && yyjson_is_uint(b)) sum += yyjson_get_uint(b);
  }
  yyjson_doc_free(doc);
  return sum;
}