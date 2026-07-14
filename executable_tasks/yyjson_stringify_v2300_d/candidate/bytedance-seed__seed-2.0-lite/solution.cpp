#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val* root = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root);

    for (size_t i = 0; i < ids.size(); ++i) {
        yyjson_mut_val* obj = yyjson_mut_obj(doc);
        // Add fields in exact original order to preserve field sequence
        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        const bool flag = (ids[i] % 3U) == 0U;
        yyjson_mut_obj_add_bool(doc, obj, "flag", flag);
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        // Pass precomputed string length to avoid internal strlen call
        yyjson_mut_obj_add_strn(doc, obj, "name", names[i].data(), names[i].size());
        yyjson_mut_arr_append(root, obj);
    }

    size_t json_len;
    char* json_buf = yyjson_write(doc, YYJSON_WRITE_MINIFY, &json_len);
    std::string result(json_buf, json_len);
    
    yyjson_free(json_buf);
    yyjson_mut_doc_free(doc);
    
    return result;
}