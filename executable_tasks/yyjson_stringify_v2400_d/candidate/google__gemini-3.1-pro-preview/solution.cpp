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

    // Pre-allocate keys to avoid repeated string processing if possible,
    // though yyjson_mut_obj_add_* functions are already quite fast.
    for (size_t i = 0; i < count; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        
        // Add fields in the exact required order
        // id: uint32_t
        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        
        // flag: bool (ids[i] % 3 == 0)
        yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
        
        // value: uint32_t
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        
        // name: string
        // Note: names[i].c_str() is safe as we are building the doc immediately
        yyjson_mut_obj_add_str(doc, obj, "name", names[i].c_str());

        // Append object to array
        yyjson_mut_arr_append(root, obj);
    }

    // Write to minified JSON string
    size_t len = 0;
    char *json_ptr = yyjson_mut_write(doc, 0, &len);
    
    std::string result;
    if (json_ptr) {
        result.assign(json_ptr, len);
        // yyjson_mut_write allocates memory using the doc's allocator (default malloc)
        free(json_ptr);
    }

    // Clean up document
    yyjson_mut_doc_free(doc);

    return result;
}