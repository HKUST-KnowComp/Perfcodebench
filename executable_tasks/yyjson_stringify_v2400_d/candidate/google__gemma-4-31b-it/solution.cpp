#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    yyjson_doc *doc = yyjson_doc_new(NULL);
    size_t count = ids.size();
    
    yyjson_val *arr = yyjson_arr_new(doc, count);
    
    for (size_t i = 0; i < count; ++i) {
        yyjson_val *obj = yyjson_obj_new(doc);
        
        // Preserve field order: id, flag, value, name
        yyjson_obj_add_int(obj, "id", ids[i]);
        yyjson_obj_add_bool(obj, "flag", (ids[i] % 3U) == 0U);
        yyjson_obj_add_int(obj, "value", values[i]);
        yyjson_obj_add_str(obj, "name", names[i].c_str());
        
        yyjson_arr_add_val(arr, obj);
    }
    
    yyjson_doc_set_root(doc, arr);
    
    // Write to a buffer
    yyjson_write_options_t opts = {0};
    char *json_str = yyjson_write(doc, &opts, 0);
    
    std::string result(json_str);
    
    yyjson_free_write(json_str);
    yyjson_doc_free(doc);
    
    return result;
}