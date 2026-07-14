#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    yyjson_doc *doc = yyjson_doc_new(YYJSON_DOC_BUILD_OPTS);
    yyjson_mut_val *arr = yyjson_mut_array(doc);

    for (std::size_t i = 0; i < ids.size(); ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        
        // Field order must be preserved: id, flag, value, name
        yyjson_mut_obj_add(obj, "id", yyjson_mut_uint(doc, ids[i]));
        yyjson_mut_obj_add(obj, "flag", yyjson_mut_bool(doc, (ids[i] % 3U) == 0U));
        yyjson_mut_obj_add(obj, "value", yyjson_mut_uint(doc, values[i]));
        yyjson_mut_obj_add(obj, "name", yyjson_mut_str(doc, names[i].c_str()));
        
        yyjson_mut_arr_add(arr, obj);
    }

    yyjson_mut_write_opts opts = {0};
    char *json_str = yyjson_mut_write(doc, opts);
    
    std::string result(json_str);
    
    yyjson_free_string(json_str);
    yyjson_doc_free(doc);
    
    return result;
}