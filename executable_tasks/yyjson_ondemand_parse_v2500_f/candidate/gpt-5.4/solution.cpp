#include "interface.h"

#include <cstdint>
#include <string>

#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_read_err err;
  yyjson_doc* doc = yyjson_read_opts(
      json_input.data(),
      json_input.size(),
      YYJSON_READ_NOFLAG,
      nullptr,
      &err);
  if (!doc) return 0;

  uint64_t sum = 0;
  yyjson_val* root = yyjson_doc_get_root(doc);

  if (yyjson_is_arr(root)) {
    size_t idx, max;
    yyjson_val* item;
    yyjson_arr_foreach(root, idx, max, item) {
      if (!yyjson_is_obj(item)) continue;

      yyjson_val* id = yyjson_obj_get(item, "id");
      if (yyjson_is_uint(id)) {
        sum += yyjson_get_uint(id);
      } else if (yyjson_is_sint(id)) {
        sum += static_cast<uint64_t>(yyjson_get_sint(id));
      }

      yyjson_val* nested = yyjson_obj_get(item, "nested");
      if (!yyjson_is_obj(nested)) continue;

      yyjson_val* a = yyjson_obj_get(nested, "a");
      if (yyjson_is_uint(a)) {
        sum += yyjson_get_uint(a);
      } else if (yyjson_is_sint(a)) {
        sum += static_cast<uint64_t>(yyjson_get_sint(a));
      }

      yyjson_val* b = yyjson_obj_get(nested, "b");
      if (yyjson_is_uint(b)) {
        sum += yyjson_get_uint(b);
      } else if (yyjson_is_sint(b)) {
        sum += static_cast<uint64_t>(yyjson_get_sint(b));
      }
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
