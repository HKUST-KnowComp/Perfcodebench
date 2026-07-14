#include "interface.h"
#include <yyjson.h>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    // Initialize yyjson mutable document and root array
    yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val* root = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root);

    for (size_t i = 0; i < ids.size(); ++i) {
        yyjson_mut_val* obj = yyjson_mut_obj(doc);
        // Add fields in exact original order to preserve correctness requirements
        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U == 0U));
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        yyjson_mut_obj_add_str(doc, obj, "name", names[i].c_str());
        yyjson_mut_arr_append(root, obj);
    }

    // Serialize to minified JSON string matching baseline output
    size_t json_len;
    char* json_cstr = yyjson_write_str(doc, YYJSON_WRITE_MINIFY, &json_len);
    std::string result(json_cstr, json_len);

    // Clean up all allocated memory to avoid leaks
    free(json_cstr);
    yyjson_mut_doc_free(doc);

    return result;
}