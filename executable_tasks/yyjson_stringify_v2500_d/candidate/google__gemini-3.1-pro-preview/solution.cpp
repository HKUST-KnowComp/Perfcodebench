#include "interface.h"
#include "yyjson.h"
#include <string>
#include <vector>
#include <cstdlib>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    if (!doc) return "";

    yyjson_mut_val *arr = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, arr);
    
    size_t len = ids.size();
    for (size_t i = 0; i < len; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        yyjson_mut_obj_add_strn(doc, obj, "name", names[i].data(), names[i].size());
        yyjson_mut_arr_append(arr, obj);
    }
    
    size_t out_len = 0;
    char *json_str = yyjson_mut_write(doc, 0, &out_len);
    
    std::string result;
    if (json_str) {
        result.assign(json_str, out_len);
        free(json_str);
    }
    
    yyjson_mut_doc_free(doc);
    
    return result;
}
