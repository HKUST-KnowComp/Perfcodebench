#include "interface.h"

#include <yyjson.h>

#include <string>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
  if (!doc) {
    return 0;
  }

  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;

  size_t idx, max;
  yyjson_val* item;
  yyjson_arr_foreach(root, idx, max, item) {
    yyjson_val* flag_val = yyjson_obj_get(item, "flag");
    if (!flag_val || !yyjson_get_bool(flag_val)) {
      continue;
    }

    yyjson_val* id_val = yyjson_obj_get(item, "id");
    yyjson_val* value_val = yyjson_obj_get(item, "value");
    yyjson_val* name_val = yyjson_obj_get(item, "name");

    if (id_val) {
      sum += yyjson_get_uint(id_val);
    }
    if (value_val) {
      sum += yyjson_get_uint(value_val);
    }
    if (name_val) {
      size_t len;
      yyjson_get_str(name_val, &len);
      sum += static_cast<uint64_t>(len);
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
