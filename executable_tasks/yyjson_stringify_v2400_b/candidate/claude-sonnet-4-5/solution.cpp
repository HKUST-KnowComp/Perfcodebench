#include "interface.h"

#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
  yyjson_mut_val* arr = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, arr);

  for (std::size_t i = 0; i < ids.size(); ++i) {
    yyjson_mut_val* obj = yyjson_mut_obj(doc);
    
    yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
    yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
    yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
    yyjson_mut_obj_add_strncpy(doc, obj, "name", names[i].c_str(), names[i].size());
    
    yyjson_mut_arr_append(arr, obj);
  }

  size_t len;
  char* json_str = yyjson_mut_write(doc, YYJSON_WRITE_NOFLAG, &len);
  std::string result(json_str, len);
  free(json_str);
  yyjson_mut_doc_free(doc);
  
  return result;
}