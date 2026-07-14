#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    yyjson_doc *doc = yyjson_doc_new(NULL);
    yyjson_val *root = yyjson_arr_new(doc);

    size_t n = ids.size();
    for (size_t i = 0; i < n; ++i) {
        yyjson_val *obj = yyjson_obj_new(doc);
        
        // Field order must be preserved: id, flag, value, name
        yyjson_obj_add_int(obj, "id", ids[i]);
        yyjson_obj_add_bool(obj, "flag", (ids[i] % 3U) == 0U);
        yyjson_obj_add_int(obj, "value", values[i]);
        yyjson_obj_add_str(obj, "name", names[i].c_str());
        
        yyjson_arr_add(root, obj);
    }

    yyjson_mut_add_val(doc, root);

    // Write to a buffer
    yyjson_write_options_t opts = {0};
    char *json_str = yyjson_write_string(doc, root, &opts);
    
    std::string result(json_str);
    
    // Cleanup
    free(json_str);
    yyjson_doc_free(doc);
    
    return result;
}