#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
  if (!doc) return 0;
  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;
  yyjson_arr_iter iter;
  yyjson_arr_iter_init(root, &iter);
  yyjson_val* item;
  while ((item = yyjson_arr_iter_next(&iter))) {
    yyjson_val* id_val = yyjson_obj_getn(item, "id", 2);
    if (id_val) sum += yyjson_get_uint(id_val);
    yyjson_val* nested_val = yyjson_obj_getn(item, "nested", 6);
    if (nested_val) {
      yyjson_val* a_val = yyjson_obj_getn(nested_val, "a", 1);
      yyjson_val* b_val = yyjson_obj_getn(nested_val, "b", 1);
      if (a_val) sum += yyjson_get_uint(a_val);
      if (b_val) sum += yyjson_get_uint(b_val);
    }
  }
  yyjson_doc_free(doc);
  return sum;
}