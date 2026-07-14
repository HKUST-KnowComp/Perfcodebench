#include "interface.h"

#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
  if (!doc) return std::string();

  yyjson_mut_val *arr = yyjson_mut_arr(doc);
  if (!arr) {
    yyjson_mut_doc_free(doc);
    return std::string();
  }
  yyjson_mut_doc_set_root(doc, arr);

  const size_t n = ids.size();
  for (size_t i = 0; i < n; ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj(doc);
    if (!obj) {
      yyjson_mut_doc_free(doc);
      return std::string();
    }

    // id
    yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "id"), yyjson_mut_uint(doc, static_cast<uint64_t>(ids[i])));

    // flag: true if id % 3 == 0
    bool flg = ((ids[i] % 3u) == 0u);
    yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "flag"), yyjson_mut_bool(doc, flg));

    // value
    yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "value"), yyjson_mut_uint(doc, static_cast<uint64_t>(values[i])));

    // name
    const std::string &nm = names[i];
    yyjson_mut_obj_add(obj, yyjson_mut_str(doc, "name"), yyjson_mut_strn(doc, nm.data(), nm.size()));

    yyjson_mut_arr_add(arr, obj);
  }

  size_t len = 0;
  char *json = yyjson_mut_write(doc, 0, &len);
  yyjson_mut_doc_free(doc);

  if (!json) return std::string();
  std::string out(json, len);
  free(json);
  return out;
}
