#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read_opts((const char*)json_input.data(), json_input.size(), 0, NULL, NULL);
  if (!doc) return 0;
  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;
  yyjson_val* arr;
  yyjson_arr_iter iter;
  yyjson_val* item;
  yyjson_arr_iter_init(root, &iter);
  while ((item = yyjson_arr_iter_next(&iter))) {
    yyjson_val* id = yyjson_obj_get(item, "id");
    yyjson_val* nested = yyjson_obj_get(item, "nested");
    yyjson_val* a = yyjson_obj_get(nested, "a");
    yyjson_val* b = yyjson_obj_get(nested, "b");
    if (id && yyjson_is_num(id) && a && yyjson_is_num(a) && b && yyjson_is_num(b)) {
      sum += yyjson_get_uint(id);
      sum += yyjson_get_uint(a);
      sum += yyjson_get_uint(b);
    }
  }
  yyjson_doc_free(doc);
  return sum;
}