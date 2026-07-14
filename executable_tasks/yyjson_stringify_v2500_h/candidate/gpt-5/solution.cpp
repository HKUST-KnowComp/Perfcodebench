#include "interface.h"

#include <yyjson.h>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();

  yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
  if (!doc) {
    // Fallback (unlikely): return empty array to preserve correctness contract
    return std::string("[]");
  }

  yyjson_mut_val* arr = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, arr);

  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val* obj = yyjson_mut_obj(doc);

    // Preserve field order: id, flag, value, name
    yyjson_mut_obj_add_uint(doc, obj, "id", static_cast<uint64_t>(ids[i]));
    yyjson_mut_obj_add_bool(doc, obj, "flag", ((ids[i] % 3U) == 0U));
    yyjson_mut_obj_add_uint(doc, obj, "value", static_cast<uint64_t>(values[i]));
    const std::string& name = names[i];
    yyjson_mut_obj_add_strn(doc, obj, "name", name.c_str(), name.size());

    yyjson_mut_arr_add(arr, obj);
  }

  size_t len = 0;
  char* json = yyjson_mut_write(doc, 0, &len);

  std::string out;
  if (json) {
    out.assign(json, len);
    free(json);
  } else {
    // Fallback if write fails
    out = "[]";
  }

  yyjson_mut_doc_free(doc);
  return out;
}
