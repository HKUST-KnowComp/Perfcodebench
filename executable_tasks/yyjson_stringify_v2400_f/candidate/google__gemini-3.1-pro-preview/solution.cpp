#include "interface.h"
#include "yyjson.h"
#include <vector>
#include <string>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    const size_t count = ids.size();
    
    // Create a mutable document
    yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
    if (!doc) return "";

    // Create the root array
    yyjson_val *root = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root);

    // Iterate and build objects
    for (size_t i = 0; i < count; ++i) {
        yyjson_val *obj = yyjson_mut_obj(doc);
        // Append object to array first to maintain structure
        yyjson_mut_arr_append(root, obj);

        // Add fields in the exact order required
        // 1. id
        yyjson_mut_obj_add_uint(doc, obj, "id", (uint64_t)ids[i]);
        
        // 2. flag
        yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
        
        // 3. value
        yyjson_mut_obj_add_uint(doc, obj, "value", (uint64_t)values[i]);
        
        // 4. name
        // Use strn to handle potential null characters and avoid extra strlen
        yyjson_mut_obj_add_strn(doc, obj, "name", names[i].c_str(), names[i].size());
    }

    // Write to minified JSON string
    size_t len = 0;
    yyjson_write_err err;
    char *json_ptr = yyjson_mut_write_opts(doc, 0, nullptr, &len, &err);

    std::string result;
    if (json_ptr) {
        result.assign(json_ptr, len);
        // yyjson_mut_write returns a buffer allocated with the doc's allocator (default: malloc)
        free(json_ptr);
    }

    // Clean up document
    yyjson_mut_doc_free(doc);

    return result;
}