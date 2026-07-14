#include "interface.h"
#include "yyjson.h"
#include <string>
#include <vector>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    const size_t count = ids.size();
    if (count == 0) {
        return "[]";
    }

    // Initialize yyjson mutable document
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *arr = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, arr);

    // Pre-allocate memory in the document for the objects to reduce reallocations
    // Each record has 1 object, 4 keys, and 4 values.
    // yyjson_mut_arr_add_obj and yyjson_mut_obj_add_* handle the internal linked list/array growth.

    for (size_t i = 0; i < count; ++i) {
        yyjson_mut_val *obj = yyjson_mut_arr_add_obj(doc, arr);
        
        // Field order: id, flag, value, name
        // yyjson_mut_obj_add_* functions append fields in the order they are called.
        
        // "id": uint32
        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        
        // "flag": bool (ids[i] % 3 == 0)
        yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
        
        // "value": uint32
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        
        // "name": string
        // Use the string length to avoid strlen inside yyjson
        yyjson_mut_obj_add_strn(doc, obj, "name", names[i].c_str(), names[i].length());
    }

    // Write the document to a minified JSON string
    size_t len = 0;
    char *json_ptr = yyjson_mut_write(doc, 0, &len);
    
    std::string result;
    if (json_ptr) {
        result.assign(json_ptr, len);
        // yyjson_mut_write returns a buffer allocated with the doc's allocator (default: malloc)
        free(json_ptr);
    }

    // Clean up the document
    yyjson_mut_doc_free(doc);

    return result;
}