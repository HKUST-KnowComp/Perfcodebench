#include "interface.h"

#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  const std::size_t n = ids.size();
  
  // Create mutable document with pre-allocated capacity
  yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
  yyjson_mut_val* arr = yyjson_mut_arr_with_capacity(doc, n);
  yyjson_mut_doc_set_root(doc, arr);
  
  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val* obj = yyjson_mut_obj_with_capacity(doc, 4);
    
    // Add "id" field
    yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
    
    // Add "flag" field
    yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
    
    // Add "value" field
    yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
    
    // Add "name" field
    yyjson_mut_obj_add_strncpy(doc, obj, "name", names[i].c_str(), names[i].size());
    
    yyjson_mut_arr_append(arr, obj);
  }
  
  // Write to string (minified)
  size_t json_len;
  char* json_str = yyjson_mut_write(doc, 0, &json_len);
  
  std::string result(json_str, json_len);
  
  // Free resources
  free(json_str);
  yyjson_mut_doc_free(doc);
  
  return result;
}