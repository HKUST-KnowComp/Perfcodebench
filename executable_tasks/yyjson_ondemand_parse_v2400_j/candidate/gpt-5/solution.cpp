#include "interface.h"
#include "yyjson.h"

#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;
  if (json_input.empty()) return 0;

  yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
  if (!doc) return 0;

  yyjson_val *root = yyjson_doc_get_root(doc);
  if (yyjson_is_arr(root)) {
    size_t idx, max;
    yyjson_val *item;
    yyjson_arr_foreach(root, idx, max, item) {
      if (!yyjson_is_obj(item)) continue;

      yyjson_val *idv = yyjson_obj_getn(item, "id", 2);
      if (idv && yyjson_is_num(idv)) {
        sum += yyjson_get_uint(idv);
      }

      yyjson_val *nested = yyjson_obj_getn(item, "nested", 6);
      if (nested && yyjson_is_obj(nested)) {
        yyjson_val *av = yyjson_obj_getn(nested, "a", 1);
        if (av && yyjson_is_num(av)) sum += yyjson_get_uint(av);
        yyjson_val *bv = yyjson_obj_getn(nested, "b", 1);
        if (bv && yyjson_is_num(bv)) sum += yyjson_get_uint(bv);
      }
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
