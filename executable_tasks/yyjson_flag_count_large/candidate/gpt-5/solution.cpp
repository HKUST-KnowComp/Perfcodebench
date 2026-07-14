#include "interface.h"

#include <yyjson.h>

#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_read_err err;
  yyjson_doc* doc = yyjson_read_opts(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG, nullptr, &err);
  if (!doc) {
    return 0;
  }

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

    yyjson_val* flagv = yyjson_obj_get(item, "flag");
    if (!flagv || !yyjson_is_bool(flagv) || !yyjson_get_bool(flagv)) continue;

    yyjson_val* idv = yyjson_obj_get(item, "id");
    if (idv) {
      if (yyjson_is_uint(idv)) {
        sum += yyjson_get_uint(idv);
      } else if (yyjson_is_sint(idv)) {
        sum += static_cast<uint64_t>(yyjson_get_sint(idv));
      }
    }

    yyjson_val* valv = yyjson_obj_get(item, "value");
    if (valv) {
      if (yyjson_is_uint(valv)) {
        sum += yyjson_get_uint(valv);
      } else if (yyjson_is_sint(valv)) {
        sum += static_cast<uint64_t>(yyjson_get_sint(valv));
      }
    }

    yyjson_val* namev = yyjson_obj_get(item, "name");
    if (namev && yyjson_is_str(namev)) {
      sum += static_cast<uint64_t>(yyjson_get_len(namev));
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
