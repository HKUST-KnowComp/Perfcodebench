#include "interface.h"

#include "yyjson.h"

#include <cstdlib>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
  if (doc == nullptr) {
    return {};
  }
  yyjson_mut_val* arr = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, arr);

  for (std::size_t i = 0; i < ids.size(); ++i) {
    yyjson_mut_val* obj = yyjson_mut_arr_add_obj(doc, arr);
    yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
    yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
    yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
    yyjson_mut_obj_add_strncpy(doc, obj, "name", names[i].data(), names[i].size());
  }

  size_t len = 0;
  char* out = yyjson_mut_write(doc, 0, &len);
  std::string result;
  if (out != nullptr) {
    result.assign(out, len);
    std::free(out);
  }
  yyjson_mut_doc_free(doc);
  return result;
}
