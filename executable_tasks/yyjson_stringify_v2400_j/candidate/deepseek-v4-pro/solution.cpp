#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  // Create mutable JSON document
  yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
  yyjson_mut_val* arr = yyjson_mut_arr(doc);
  const std::size_t size = ids.size();
  if (size > 0) {
    yyjson_mut_arr_reserve(doc, arr, size);
    for (std::size_t i = 0; i < size; ++i) {
      yyjson_mut_val* obj = yyjson_mut_obj(doc);
      yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
      yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
      yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
      yyjson_mut_obj_add_strncpy(doc, obj, "name", names[i].c_str(), names[i].size());
      yyjson_mut_arr_append(arr, obj);
    }
  }
  yyjson_mut_doc_set_root(doc, arr);

  // Serialize to minified JSON
  std::size_t len = 0;
  char* json = yyjson_mut_write(doc, 0, &len);
  std::string result(json, len);
  std::free(json);
  yyjson_mut_doc_free(doc);
  return result;
}