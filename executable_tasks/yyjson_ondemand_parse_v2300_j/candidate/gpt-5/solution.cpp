#include "interface.h"
#include "yyjson.h"

#include <cstddef>

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;

  yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
  if (!doc) return 0;

  yyjson_val *root = yyjson_doc_get_root(doc);
  if (!root || !yyjson_is_arr(root)) {
    yyjson_doc_free(doc);
    return 0;
  }

  size_t idx, max;
  yyjson_val *item;
  yyjson_arr_foreach(root, idx, max, item) {
    yyjson_val *v_id = yyjson_obj_getn(item, "id", 2);
    sum += yyjson_get_uint(v_id);

    yyjson_val *nested = yyjson_obj_getn(item, "nested", 6);
    if (nested && yyjson_is_obj(nested)) {
      yyjson_val *v_a = yyjson_obj_getn(nested, "a", 1);
      sum += yyjson_get_uint(v_a);

      yyjson_val *v_b = yyjson_obj_getn(nested, "b", 1);
      sum += yyjson_get_uint(v_b);
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
