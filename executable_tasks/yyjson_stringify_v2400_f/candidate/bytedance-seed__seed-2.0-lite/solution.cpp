#include "interface.h"
#include <yyjson.h>
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    // Create mutable yyjson document
    yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
    // Initialize root array matching baseline structure
    yyjson_mut_val *root = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root);

    const size_t n = ids.size();
    for (size_t i = 0; i < n; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        // Add fields in exact original order to preserve output correctness
        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        const std::string& name = names[i];
        yyjson_mut_obj_add_str(doc, obj, "name", name.data(), name.size());
        yyjson_mut_arr_append(root, obj);
    }

    // Generate minified JSON (0 flags = default compact output)
    size_t json_len;
    char *json_buf = yyjson_write(doc, 0, &json_len);
    // Copy output to std::string for return
    std::string result(json_buf, json_len);
    // Clean up yyjson allocated resources
    yyjson_free(json_buf);
    yyjson_mut_doc_free(doc);

    return result;
}