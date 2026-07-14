#include "interface.h"

#include <cstdint>
#include <string>
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;

  yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
  if (!doc) return 0;

  yyjson_val* root = yyjson_doc_get_root(doc);
  if (!root || !yyjson_is_arr(root)) {
    yyjson_doc_free(doc);
    return 0;
  }

  size_t idx, max;
  yyjson_val* item;
  yyjson_arr_foreach(root, idx, max, item) {
    if (!yyjson_is_obj(item)) continue;

    // id
    sum += yyjson_get_uint(yyjson_obj_getn(item, "id", 2));

    // nested.a and nested.b
    yyjson_val* nested = yyjson_obj_getn(item, "nested", 6);
    if (yyjson_is_obj(nested)) {
      sum += yyjson_get_uint(yyjson_obj_getn(nested, "a", 1));
      sum += yyjson_get_uint(yyjson_obj_getn(nested, "b", 1));
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
