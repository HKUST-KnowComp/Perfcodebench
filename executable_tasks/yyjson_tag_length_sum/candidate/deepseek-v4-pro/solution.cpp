#include "interface.h"

#include <yyjson.h>

#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
  yyjson_val *root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;

  size_t idx, max;
  yyjson_val *item;
  yyjson_arr_foreach(root, idx, max, item) {
    yyjson_val *flag_val = yyjson_obj_get(item, "flag");
    if (!yyjson_get_bool(flag_val)) {
      continue;
    }

    yyjson_val *id_val = yyjson_obj_get(item, "id");
    sum += yyjson_get_uint(id_val);

    yyjson_val *tag_val = yyjson_obj_get(item, "tag");
    sum += static_cast<uint64_t>(yyjson_get_len(tag_val));

    yyjson_val *value_val = yyjson_obj_get(item, "value");
    sum += yyjson_get_uint(value_val);
  }

  yyjson_doc_free(doc);
  return sum;
}