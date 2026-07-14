#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    yyjson_doc *doc = yyjson_doc_new(NULL);
    yyjson_mut_val *arr = yyjson_mut_arr(doc);

    for (std::size_t i = 0; i < ids.size(); ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        
        // Field order must be preserved: id, flag, value, name
        yyjson_mut_obj_add_int(obj, "id", ids[i]);
        yyjson_mut_obj_add_bool(obj, "flag", (ids[i] % 3U) == 0U);
        yyjson_mut_obj_add_int(obj, "value", values[i]);
        yyjson_mut_obj_add_str(obj, "name", names[i].c_str());
        
        yyjson_mut_arr_add_val(arr, obj);
    }

    yyjson_mut_val *root = yyjson_mut_doc_get_root(doc);
    char *json_str = yyjson_write_string(doc, root, 0);
    
    std::string result(json_str);
    
    free(json_str);
    yyjson_doc_free(doc);
    
    return result;
}