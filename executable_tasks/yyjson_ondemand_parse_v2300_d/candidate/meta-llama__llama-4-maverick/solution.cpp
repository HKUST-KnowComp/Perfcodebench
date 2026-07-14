#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read_opts((const char*)json_input.data(), json_input.size(), 0, NULL, NULL);
  if (!doc) return 0;
  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;
  size_t idx, max;
  yyjson_val *item, *id, *nested, *a, *b;
  yyjson_arr_iter_iterate(root, idx, max, item) {
    id = yyjson_obj_get(item, "id");
    nested = yyjson_obj_get(item, "nested");
    if (id && nested) {
      a = yyjson_obj_get(nested, "a");
      b = yyjson_obj_get(nested, "b");
      if (id->type == YYJSON_TYPE_NUM && a->type == YYJSON_TYPE_NUM && b->type == YYJSON_TYPE_NUM) {
        sum += yyjson_get_uint(id);
        sum += yyjson_get_uint(a);
        sum += yyjson_get_uint(b);
      }
    }
  }
  yyjson_doc_free(doc);
  return sum;
}