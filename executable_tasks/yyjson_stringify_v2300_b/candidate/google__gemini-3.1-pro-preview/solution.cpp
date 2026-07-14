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

    // Create a mutable doc for building the JSON structure
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *arr = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, arr);

    for (size_t i = 0; i < count; ++i) {
        yyjson_mut_val *obj = yyjson_mut_obj(doc);
        
        // Add fields in the exact required order: id, flag, value, name
        // yyjson_mut_obj_add_* macros handle key creation and value assignment
        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        yyjson_mut_obj_add_bool(doc, obj, "flag", ((ids[i] % 3U) == 0U));
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        
        // Use the string length to avoid unnecessary strlen calls
        const std::string& name_str = names[i];
        yyjson_mut_obj_add_strn(doc, obj, "name", name_str.c_str(), name_str.length());
        
        // Append the object to the root array
        yyjson_mut_arr_append(arr, obj);
    }

    // Write the document to a minified JSON string
    size_t len = 0;
    char *json_ptr = yyjson_mut_write(doc, 0, &len);
    
    std::string result;
    if (json_ptr) {
        result.assign(json_ptr, len);
        // yyjson_mut_write allocates memory using the doc's allocator (default: malloc)
        free(json_ptr);
    }

    // Clean up the document
    yyjson_mut_doc_free(doc);

    return result;
}