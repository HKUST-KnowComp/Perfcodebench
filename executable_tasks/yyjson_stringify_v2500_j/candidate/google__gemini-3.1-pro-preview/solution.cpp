#include "interface.h"
#include "yyjson.h"
#include <vector>
#include <string>
#include <cstdint>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    size_t count = ids.size();
    if (count == 0) {
        return "[]";
    }

    // Create a mutable document
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    if (!doc) return "";

    // Create the root array
    yyjson_mut_val *root = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root);

    // Pre-allocate/reserve space in the array if possible (yyjson internal optimization)
    // Note: yyjson_mut_arr_append is efficient, but we can build objects and add them.
    
    for (size_t i = 0; i < count; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        
        // Add fields in the exact required order
        // "id"
        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        
        // "flag"
        yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
        
        // "value"
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        
        // "name"
        // Use strn to handle potential nulls or just use the string pointer
        yyjson_mut_obj_add_strncpy(doc, obj, "name", names[i].c_str(), names[i].size());

        // Append object to root array
        yyjson_mut_arr_append(root, obj);
    }

    // Write the JSON to a string
    size_t len = 0;
    char *json_str = yyjson_mut_write(doc, 0, &len);
    
    std::string result;
    if (json_str) {
        result.assign(json_str, len);
        // yyjson_mut_write returns a buffer allocated with the doc's allocator (default: malloc)
        free(json_str);
    }

    // Clean up the document
    yyjson_mut_doc_free(doc);

    return result;
}