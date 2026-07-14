#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
  if (!doc) return 0;

  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;

  size_t arr_len;
  yyjson_val* const* arr = yyjson_arr_get_all(root, &arr_len);
  for (size_t i = 0; i < arr_len; i++) {
    yyjson_val* item = arr[i];
    sum += yyjson_get_uint(yyjson_obj_get(item, "id"));
    yyjson_val* nested = yyjson_obj_get(item, "nested");
    sum += yyjson_get_uint(yyjson_obj_get(nested, "a"));
    sum += yyjson_get_uint(yyjson_obj_get(nested, "b"));
  }

  yyjson_doc_free(doc);
  return sum;
}