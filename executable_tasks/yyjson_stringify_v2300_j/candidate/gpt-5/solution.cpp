#include "interface.h"

#include <yyjson.h>
#include <cstdlib>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const size_t n = ids.size();

  yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
  if (!doc) return std::string("[]");

  yyjson_mut_val *arr = yyjson_mut_arr_with_capacity(doc, (uint32_t)n);
  yyjson_mut_doc_set_root(doc, arr);

  for (size_t i = 0; i < n; ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj_with_capacity(doc, 4);

    // id
    yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "id"), yyjson_mut_uint(doc, (uint64_t)ids[i]));
    // flag
    const bool flag = ((ids[i] % 3u) == 0u);
    yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "flag"), flag ? yyjson_mut_true(doc) : yyjson_mut_false(doc));
    // value
    yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "value"), yyjson_mut_uint(doc, (uint64_t)values[i]));
    // name
    const std::string &name = names[i];
    yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "name"), yyjson_mut_strn(doc, name.data(), name.size()));

    yyjson_mut_arr_add(arr, obj);
  }

  size_t len = 0;
  char *json = yyjson_mut_write(doc, 0, &len);

  std::string out;
  if (json) {
    out.assign(json, len);
    free(json);
  } else {
    // Fallback: should not occur under normal circumstances
    out = "[]";
  }

  yyjson_mut_doc_free(doc);
  return out;
}
