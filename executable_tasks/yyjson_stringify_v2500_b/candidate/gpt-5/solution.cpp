#include "interface.h"

#include <cstdlib>
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const size_t n = ids.size();
  if (n == 0) return "[]";

  yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
  yyjson_mut_val *arr = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, arr);

  for (size_t i = 0; i < n; ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj(doc);

    // Add fields in required order: id, flag, value, name
    yyjson_mut_obj_add_uint(doc, obj, "id", static_cast<uint64_t>(ids[i]));
    bool flag = ((ids[i] % 3u) == 0u);
    yyjson_mut_obj_add_bool(doc, obj, "flag", flag);
    yyjson_mut_obj_add_uint(doc, obj, "value", static_cast<uint64_t>(values[i]));
    const std::string &s = names[i];
    yyjson_mut_obj_add_strn(doc, obj, "name", s.data(), s.size());

    yyjson_mut_arr_add(arr, obj);
  }

  size_t len = 0;
  char *json = yyjson_mut_write(doc, 0, &len);

  std::string out;
  if (json && len != 0) {
    out.assign(json, len);
  } else {
    out = "[]";
  }

  std::free(json);
  yyjson_mut_doc_free(doc);
  return out;
}
