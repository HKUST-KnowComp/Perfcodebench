#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t n = ids.size();
    yyjson_doc *doc = yyjson_doc_new(YYJSON_WRITE_ANY);
    yyjson_val *root = yyjson_doc_get_root(doc);
    
    // Create the array
    yyjson_val *arr = yyjson_arr(root);
    
    for (size_t i = 0; i < n; ++i) {
        yyjson_val *obj = yyjson_obj(arr);
        
        // yyjson maintains insertion order for objects, satisfying the correctness rule
        yyjson_obj_add_uint(obj, "id", ids[i]);
        yyjson_obj_add_bool(obj, "flag", (ids[i] % 3U) == 0U);
        yyjson_obj_add_uint(obj, "value", values[i]);
        yyjson_obj_add_str(obj, "name", names[i].c_str());
    }

    // Serialize to minified string
    const char *json_str = yyjson_doc_get_cstr(doc);
    std::string result = json_str ? json_str : "[]";

    yyjson_doc_free(doc);
    return result;
}