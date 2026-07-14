#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  yyjson_mut_doc doc;
  yyjson_mut_arr root = yyjson_mut_arr_new(&doc);
  yyjson_mut_obj obj;
  for (std::size_t i = 0; i < ids.size(); ++i) {
    obj = yyjson_mut_obj_new(&doc);
    yyjson_mut_obj_set_uint(obj, "id", ids[i]);
    yyjson_mut_obj_set_bool(obj, "flag", (ids[i] % 3U) == 0U);
    yyjson_mut_obj_set_uint(obj, "value", values[i]);
    yyjson_mut_obj_set_str(obj, "name", names[i].c_str());
    yyjson_mut_arr_append(root, obj);
  }
  const char* json_str;
  size_t json_len;
  yyjson_mut_doc_set_root(doc, root);
  yyjson_mut_doc_render(doc, &json_str, &json_len, 0);
  return std::string(json_str, json_len);
}