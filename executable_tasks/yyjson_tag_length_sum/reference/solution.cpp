#include "interface.h"

#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
  if (doc == nullptr) {
    return 0;
  }
  yyjson_val* root = yyjson_doc_get_root(doc);
  uint64_t sum = 0;
  size_t idx, max;
  yyjson_val* item;
  yyjson_arr_foreach(root, idx, max, item) {
    yyjson_val* flag = yyjson_obj_get(item, "flag");
    if (!yyjson_get_bool(flag)) {
      continue;
    }
    yyjson_val* tag = yyjson_obj_get(item, "tag");
    sum += yyjson_get_uint(yyjson_obj_get(item, "id"));
    sum += static_cast<uint64_t>(yyjson_get_len(tag));
    sum += yyjson_get_uint(yyjson_obj_get(item, "value"));
  }
  yyjson_doc_free(doc);
  return sum;
}
