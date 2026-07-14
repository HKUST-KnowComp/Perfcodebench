#include "interface.h"

#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
  std::size_t n = ids.size();
  
  // Create mutable document with pre-allocated capacity
  yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
  yyjson_mut_val* arr = yyjson_mut_arr(doc);
  yyjson_mut_doc_set_root(doc, arr);
  
  // Reserve approximate capacity for the array
  yyjson_mut_arr_reserve(doc, arr, n);
  
  for (std::size_t i = 0; i < n; ++i) {
    yyjson_mut_val* obj = yyjson_mut_obj(doc);
    
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
  std::size_t len;
  char* json_str = yyjson_mut_write(doc, 0, &len);
  
  std::string result(json_str, len);
  
  // Free resources
  free(json_str);
  yyjson_mut_doc_free(doc);
  
  return result;
}