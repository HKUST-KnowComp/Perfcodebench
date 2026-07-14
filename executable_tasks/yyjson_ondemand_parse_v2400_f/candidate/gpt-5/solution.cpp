#include "interface.h"

#include <yyjson.h>

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;

  yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_STOP_WHEN_DONE);
  if (!doc) return 0;

  yyjson_val* root = yyjson_doc_get_root(doc);
  if (!root || !yyjson_is_arr(root)) {
    yyjson_doc_free(doc);
    return 0;
  }

  yyjson_arr_iter arr_iter;
  yyjson_arr_iter_init(root, &arr_iter);
  yyjson_val* obj;
  while ((obj = yyjson_arr_iter_next(&arr_iter))) {
    yyjson_val* v_id = yyjson_obj_getn(obj, "id", 2);
    if (v_id) sum += yyjson_get_uint(v_id);

    yyjson_val* nested = yyjson_obj_getn(obj, "nested", 6);
    if (nested && yyjson_is_obj(nested)) {
      yyjson_val* v_a = yyjson_obj_getn(nested, "a", 1);
      if (v_a) sum += yyjson_get_uint(v_a);
      yyjson_val* v_b = yyjson_obj_getn(nested, "b", 1);
      if (v_b) sum += yyjson_get_uint(v_b);
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
