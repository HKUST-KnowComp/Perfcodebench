#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    yyjson_doc *doc = yyjson_doc_new(NULL);
    size_t count = ids.size();
    
    yyjson_val *arr = yyjson_arr_open(doc);
    for (size_t i = 0; i < count; ++i) {
        yyjson_val *obj = yyjson_obj_open(doc);
        
        yyjson_obj_add_int(doc, obj, "id", ids[i]);
        yyjson_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
        yyjson_obj_add_int(doc, obj, "value", values[i]);
        yyjson_obj_add_str(doc, obj, "name", names[i].c_str());
        
        yyjson_obj_close(doc, obj);
        yyjson_arr_add_val(doc, arr, obj);
    }
    yyjson_arr_close(doc, arr);

    yyjson_mut_val *root = yyjson_doc_get_root(doc);
    char *json_str = yyjson_write_string(doc, root, 0);
    
    std::string result(json_str);
    
    yyjson_free_string(json_str);
    yyjson_doc_free(doc);
    
    return result;
}