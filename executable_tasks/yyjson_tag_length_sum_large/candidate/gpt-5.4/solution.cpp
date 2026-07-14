#include "interface.h"

#include <yyjson.h>

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_read_err err;
  yyjson_doc* doc = yyjson_read_opts(
      json_input.data(),
      json_input.size(),
      YYJSON_READ_NOFLAG,
      nullptr,
      &err);
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
    if (!yyjson_is_obj(item)) {
      continue;
    }

    yyjson_val* flag = yyjson_obj_get(item, "flag");
    if (!flag || !yyjson_is_bool(flag) || !yyjson_get_bool(flag)) {
      continue;
    }

    yyjson_val* id = yyjson_obj_get(item, "id");
    yyjson_val* tag = yyjson_obj_get(item, "tag");
    yyjson_val* value = yyjson_obj_get(item, "value");

    if (!id || !tag || !value) {
      continue;
    }
    if (!yyjson_is_uint(id) || !yyjson_is_str(tag) || !yyjson_is_uint(value)) {
      continue;
    }

    sum += yyjson_get_uint(id);
    sum += static_cast<uint64_t>(yyjson_get_len(tag));
    sum += yyjson_get_uint(value);
  }

  yyjson_doc_free(doc);
  return sum;
}
