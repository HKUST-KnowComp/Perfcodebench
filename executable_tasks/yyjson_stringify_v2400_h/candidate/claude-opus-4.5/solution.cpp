#include "interface.h"

#include <yyjson.h>
#include <cstring>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    const size_t n = ids.size();
    
    // Estimate memory needed: each object has 4 keys + 4 values + object container
    // Plus array container. Rough estimate: 64 bytes per record should be plenty.
    size_t hint = n * 128 + 4096;
    
    yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val *arr = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, arr);
    
    // Pre-create key strings (they're reused for every object)
    yyjson_mut_val *key_id = yyjson_mut_strcpy(doc, "id");
    yyjson_mut_val *key_flag = yyjson_mut_strcpy(doc, "flag");
    yyjson_mut_val *key_value = yyjson_mut_strcpy(doc, "value");
    yyjson_mut_val *key_name = yyjson_mut_strcpy(doc, "name");
    
    for (size_t i = 0; i < n; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        
        // Add "id"
        yyjson_mut_obj_add(obj, yyjson_mut_strcpy(doc, "id"), yyjson_mut_uint(doc, ids[i]));
        
        // Add "flag"
        bool flag = (ids[i] % 3U) == 0U;
        yyjson_mut_obj_add(obj, yyjson_mut_strcpy(doc, "flag"), yyjson_mut_bool(doc, flag));
        
        // Add "value"
        yyjson_mut_obj_add(obj, yyjson_mut_strcpy(doc, "value"), yyjson_mut_uint(doc, values[i]));
        
        // Add "name"
        yyjson_mut_obj_add(obj, yyjson_mut_strcpy(doc, "name"), 
                           yyjson_mut_strncpy(doc, names[i].data(), names[i].size()));
        
        yyjson_mut_arr_append(arr, obj);
    }
    
    size_t json_len = 0;
    char *json_str = yyjson_mut_write(doc, YYJSON_WRITE_NOFLAG, &json_len);
    
    std::string result;
    if (json_str) {
        result.assign(json_str, json_len);
        free(json_str);
    }
    
    yyjson_mut_doc_free(doc);
    
    return result;
}