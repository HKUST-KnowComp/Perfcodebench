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

  yyjson_val* root = yyjson_doc_get_root(doc);
  if (!root || !yyjson_is_arr(root)) {
    yyjson_doc_free(doc);
    return 0;
  }

  uint64_t sum = 0;

  size_t idx, max;
  yyjson_val* item;
  yyjson_arr_foreach(root, idx, max, item) {
    if (!yyjson_is_obj(item)) continue;

    yyjson_val* id_val = yyjson_obj_get(item, "id");
    if (id_val && yyjson_is_uint(id_val)) {
      sum += yyjson_get_uint(id_val);
    } else if (id_val && yyjson_is_sint(id_val)) {
      sum += static_cast<uint64_t>(yyjson_get_sint(id_val));
    }

    yyjson_val* nested = yyjson_obj_get(item, "nested");
    if (!nested || !yyjson_is_obj(nested)) continue;

    yyjson_val* a_val = yyjson_obj_get(nested, "a");
    if (a_val && yyjson_is_uint(a_val)) {
      sum += yyjson_get_uint(a_val);
    } else if (a_val && yyjson_is_sint(a_val)) {
      sum += static_cast<uint64_t>(yyjson_get_sint(a_val));
    }

    yyjson_val* b_val = yyjson_obj_get(nested, "b");
    if (b_val && yyjson_is_uint(b_val)) {
      sum += yyjson_get_uint(b_val);
    } else if (b_val && yyjson_is_sint(b_val)) {
      sum += static_cast<uint64_t>(yyjson_get_sint(b_val));
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
