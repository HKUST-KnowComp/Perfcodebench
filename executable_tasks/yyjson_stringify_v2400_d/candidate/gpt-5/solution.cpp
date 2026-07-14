#include "interface.h"

#include <yyjson.h>
#include <cstdlib>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
  if (!doc) return std::string("[]");

  yyjson_mut_val *arr = yyjson_mut_arr(doc);
  if (!arr) {
    yyjson_mut_doc_free(doc);
    return std::string("[]");
  }
  yyjson_mut_doc_set_root(doc, arr);

  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj(doc);
    if (!obj) {
      yyjson_mut_doc_free(doc);
      return std::string("[]");
    }

    yyjson_mut_obj_add_uint(doc, obj, "id", static_cast<uint64_t>(ids[i]));
    yyjson_mut_obj_add_bool(doc, obj, "flag", ((ids[i] % 3U) == 0U));
    yyjson_mut_obj_add_uint(doc, obj, "value", static_cast<uint64_t>(values[i]));

    const std::string &nm = names[i];
    yyjson_mut_obj_add_strn(doc, obj, "name", nm.data(), nm.size());

    yyjson_mut_arr_add(arr, obj);
  }

  size_t len = 0;
  char *out = yyjson_mut_write(doc, YYJSON_WRITE_NO_WHITESPACE, &len);
  yyjson_mut_doc_free(doc);

  if (!out) return std::string("[]");
  std::string result(out, len);
  std::free(out);
  return result;
}
