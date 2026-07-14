#include "interface.h"
#include <yyjson.h>
#include <cstdlib>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
  if (!doc) return "[]";

  yyjson_mut_val *arr = yyjson_mut_arr(doc);
  if (!arr) {
    yyjson_mut_doc_free(doc);
    return "[]";
  }

  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj(doc);
    if (!obj) {
      yyjson_mut_doc_free(doc);
      return "[]";
    }

    yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
    yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
    yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);

    yyjson_mut_val *name_val = yyjson_mut_strn(doc, names[i].data(), names[i].size());
    yyjson_mut_obj_add_val(doc, obj, "name", name_val);

    yyjson_mut_arr_append(arr, obj);
  }

  yyjson_mut_doc_set_root(doc, arr);

  char *json_cstr = yyjson_mut_write(doc, 0, NULL);
  std::string result(json_cstr ? json_cstr : "[]");
  if (json_cstr) free(json_cstr);
  yyjson_mut_doc_free(doc);
  return result;
}
