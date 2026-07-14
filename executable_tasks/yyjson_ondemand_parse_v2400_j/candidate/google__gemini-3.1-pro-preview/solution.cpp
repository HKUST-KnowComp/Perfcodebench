#include "interface.h"
#include "yyjson.h"

/**
 * Performance-optimized implementation using yyjson.
 * yyjson is a high-performance JSON library that provides faster parsing and 
 * lower memory overhead than nlohmann/json.
 */
uint64_t compute_checksum(const std::string& json_input) {
    if (json_input.empty()) return 0;

    // Read the JSON input into a DOM document.
    // Using 0 for flags as the default configuration is highly optimized.
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;

    // Iterate over the array elements.
    yyjson_val *item;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);
    
    while ((item = yyjson_arr_iter_next(&iter))) {
        // Extract "id" directly from the object.
        // yyjson_obj_get is efficient for small to medium objects.
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) {
            sum += yyjson_get_uint(id_val);
        }

        // Extract "nested" object and its children "a" and "b".
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        if (nested) {
            yyjson_val *a_val = yyjson_obj_get(nested, "a");
            if (a_val) {
                sum += yyjson_get_uint(a_val);
            }
            yyjson_val *b_val = yyjson_obj_get(nested, "b");
            if (b_val) {
                sum += yyjson_get_uint(b_val);
            }
        }
    }

    // Clean up the document memory.
    yyjson_doc_free(doc);
    
    return sum;
}