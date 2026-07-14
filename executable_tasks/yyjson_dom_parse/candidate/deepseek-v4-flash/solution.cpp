#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
  yyjson_val *root = yyjson_doc_get_root(doc);
  yyjson_arr_iter iter;
  yyjson_arr_iter_init(root, &iter);
  yyjson_val *item;
  uint64_t sum = 0;
  while ((item = yyjson_arr_iter_next(&iter))) {
    sum += yyjson_get_uint64(yyjson_obj_get(item, "id"));
    sum += yyjson_get_uint64(yyjson_obj_get(item, "x"));
    sum += yyjson_get_uint64(yyjson_arr_get(yyjson_obj_get(item, "vals"), 1));
  }
  yyjson_doc_free(doc);
  return sum;
}