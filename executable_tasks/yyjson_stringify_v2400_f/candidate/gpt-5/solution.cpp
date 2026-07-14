#include "interface.h"

#include <yyjson.h>
#include <cstdint>
#include <string>
#include <vector>
#include <cstdlib>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();
  if (n == 0) return "[]";

  yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
  yyjson_mut_val *arr = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, arr);

  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj(doc);
    // Preserve field order: id, flag, value, name
    yyjson_mut_obj_add_uint(doc, obj, "id", (uint64_t)ids[i]);
    yyjson_mut_obj_add_bool(doc, obj, "flag", ((ids[i] % 3U) == 0U));
    yyjson_mut_obj_add_uint(doc, obj, "value", (uint64_t)values[i]);
    yyjson_mut_obj_add_str(doc, obj, "name", names[i].c_str());
    yyjson_mut_arr_add(arr, obj);
  }

  size_t len = 0;
  char *json = yyjson_mut_write(doc, 0, &len);

  std::string out;
  if (json) {
    out.assign(json, len);
    free(json);
  } else {
    // Fallback: should not happen unless OOM, return empty array
    out = "[]";
  }

  yyjson_mut_doc_free(doc);
  return out;
}
