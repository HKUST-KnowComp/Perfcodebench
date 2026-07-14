#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *root_arr = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root_arr);
    
    const std::size_t count = ids.size();
    for (std::size_t i = 0; i < count; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        // Maintain exact field order to match baseline output
        yyjson_mut_obj_add_uint(obj, "id", ids[i]);
        const bool flag = (ids[i] % 3U == 0U);
        yyjson_mut_obj_add_bool(obj, "flag", flag);
        yyjson_mut_obj_add_uint(obj, "value", values[i]);
        yyjson_mut_obj_add_str(obj, "name", names[i].c_str());
        yyjson_mut_arr_append(root_arr, obj);
    }
    
    size_t json_len;
    char *json_cstr = yyjson_mut_write(doc, 0, &json_len);
    std::string result(json_cstr, json_len);
    free(json_cstr);
    yyjson_mut_doc_free(doc);
    
    return result;
}