#include "interface.h"
#include "yyjson.h"
#include <vector>
#include <string>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t n = ids.size();
    if (n == 0) {
        return "[]";
    }

    // Create a mutable document
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *arr = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, arr);

    // Pre-create keys to avoid repeated string processing and hashing
    yyjson_mut_val *key_id = yyjson_mut_str(doc, "id");
    yyjson_mut_val *key_flag = yyjson_mut_str(doc, "flag");
    yyjson_mut_val *key_value = yyjson_mut_str(doc, "value");
    yyjson_mut_val *key_name = yyjson_mut_str(doc, "name");

    for (size_t i = 0; i < n; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        
        // Add fields in the exact required order
        // id
        yyjson_mut_obj_add(obj, key_id, yyjson_mut_uint(doc, ids[i]));
        
        // flag
        yyjson_mut_obj_add(obj, key_flag, yyjson_mut_bool(doc, (ids[i] % 3U) == 0U));
        
        // value
        yyjson_mut_obj_add(obj, key_value, yyjson_mut_uint(doc, values[i]));
        
        // name
        yyjson_mut_obj_add(obj, key_name, yyjson_mut_strn(doc, names[i].data(), names[i].size()));

        // Append object to array
        yyjson_mut_arr_append(arr, obj);
    }

    // Write to minified JSON string
    size_t len = 0;
    char *json_ptr = yyjson_mut_write(doc, 0, &len);
    
    std::string result;
    if (json_ptr) {
        result.assign(json_ptr, len);
        free(json_ptr);
    }

    yyjson_mut_doc_free(doc);
    return result;
}