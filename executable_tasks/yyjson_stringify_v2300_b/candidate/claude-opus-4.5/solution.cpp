#include "interface.h"

#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
  yyjson_mut_val *arr = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, arr);

  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val *obj = yyjson_mut_obj(doc);

    // "id"
    yyjson_mut_val *key_id = yyjson_mut_strcpy(doc, "id");
    yyjson_mut_val *val_id = yyjson_mut_uint(doc, ids[i]);
    yyjson_mut_obj_add(obj, key_id, val_id);

    // "flag"
    yyjson_mut_val *key_flag = yyjson_mut_strcpy(doc, "flag");
    yyjson_mut_val *val_flag = yyjson_mut_bool(doc, (ids[i] % 3U) == 0U);
    yyjson_mut_obj_add(obj, key_flag, val_flag);

    // "value"
    yyjson_mut_val *key_value = yyjson_mut_strcpy(doc, "value");
    yyjson_mut_val *val_value = yyjson_mut_uint(doc, values[i]);
    yyjson_mut_obj_add(obj, key_value, val_value);

    // "name"
    yyjson_mut_val *key_name = yyjson_mut_strcpy(doc, "name");
    yyjson_mut_val *val_name = yyjson_mut_strncpy(doc, names[i].data(), names[i].size());
    yyjson_mut_obj_add(obj, key_name, val_name);

    yyjson_mut_arr_append(arr, obj);
  }

  size_t len = 0;
  char *json_str = yyjson_mut_write(doc, 0, &len);
  std::string result(json_str, len);
  free(json_str);
  yyjson_mut_doc_free(doc);
  return result;
}