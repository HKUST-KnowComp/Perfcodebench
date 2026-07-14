#include "interface.h"

#include <yyjson.h>

#include <cstdint>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  // Create a mutable JSON document
  yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
  yyjson_mut_val *arr = yyjson_mut_arr(doc);

  // Build array of objects
  for (std::size_t i = 0; i < ids.size(); ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj(doc);
    yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
    yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
    yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
    yyjson_mut_obj_add_strn(doc, obj, "name", names[i].c_str(), names[i].size());
    yyjson_mut_arr_append(arr, obj);
  }

  // Write compact JSON to string
  const char *json;
  size_t len;
  yyjson_mut_write(doc, YYJSON_WRITE_NOFLAG, NULL, NULL, &json, &len);

  // Copy result before freeing document
  std::string result(json, len);
  yyjson_mut_doc_free(doc);

  return result;
}