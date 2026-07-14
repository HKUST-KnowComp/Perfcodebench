#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
  if (!doc) return 0;
  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;
  yyjson_val* arr = yyjson_get_array(root);
  size_t arr_size = yyjson_arr_size(arr);
  for (size_t i = 0; i < arr_size; i++) {
    yyjson_val* obj = yyjson_arr_get(arr, i);
    yyjson_val* id = yyjson_obj_get(obj, "id");
    yyjson_val* nested = yyjson_obj_get(obj, "nested");
    if (id && nested) {
      sum += yyjson_get_uint(id);
      yyjson_val* a = yyjson_obj_get(nested, "a");
      yyjson_val* b = yyjson_obj_get(nested, "b");
      if (a) sum += yyjson_get_uint(a);
      if (b) sum += yyjson_get_uint(b);
    }
  }
  yyjson_doc_free(doc);
  return sum;
}