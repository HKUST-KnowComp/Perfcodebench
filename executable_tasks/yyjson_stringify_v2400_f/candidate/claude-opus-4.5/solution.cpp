#include "interface.h"

#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val *arr = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, arr);
    
    // Pre-create key strings (reused for all objects)
    yyjson_mut_val *key_id = yyjson_mut_strcpy(doc, "id");
    yyjson_mut_val *key_flag = yyjson_mut_strcpy(doc, "flag");
    yyjson_mut_val *key_value = yyjson_mut_strcpy(doc, "value");
    yyjson_mut_val *key_name = yyjson_mut_strcpy(doc, "name");
    
    const size_t n = ids.size();
    for (size_t i = 0; i < n; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        
        // Add fields in order: id, flag, value, name
        yyjson_mut_obj_add(obj, key_id, yyjson_mut_uint(doc, ids[i]));
        yyjson_mut_obj_add(obj, key_flag, yyjson_mut_bool(doc, (ids[i] % 3U) == 0U));
        yyjson_mut_obj_add(obj, key_value, yyjson_mut_uint(doc, values[i]));
        yyjson_mut_obj_add(obj, key_name, yyjson_mut_strncpy(doc, names[i].data(), names[i].size()));
        
        yyjson_mut_arr_append(arr, obj);
    }
    
    size_t len = 0;
    char *json_str = yyjson_mut_write(doc, 0, &len);
    
    std::string result(json_str, len);
    
    free(json_str);
    yyjson_mut_doc_free(doc);
    
    return result;
}