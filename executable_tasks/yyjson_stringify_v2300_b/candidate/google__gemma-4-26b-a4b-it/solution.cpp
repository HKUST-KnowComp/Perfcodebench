#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t n = ids.size();
    yyjson_doc *doc = yyjson_doc_new(0);
    yyjson_val *root = yyjson_arr(doc);

    for (size_t i = 0; i < n; ++i) {
        yyjson_val *obj = yyjson_obj(doc);
        
        // Field order must be preserved: id, flag, value, name
        yyjson_obj_add_uint(obj, "id", ids[i]);
        yyjson_obj_add_bool(obj, "flag", (ids[i] % 3U) == 0U);
        yyjson_obj_add_uint(obj, "value", values[i]);
        yyjson_obj_add_str(obj, "name", names[i].c_str());

        yyjson_arr_append(root, obj);
    }

    // yyjson_doc_get_str returns a pointer to the internal buffer
    // We must copy it to a std::string before freeing the doc
    const char *json_str = yyjson_doc_get_str(doc);
    std::string result = json_str ? json_str : "[]";

    yyjson_doc_free(doc);
    return result;
}