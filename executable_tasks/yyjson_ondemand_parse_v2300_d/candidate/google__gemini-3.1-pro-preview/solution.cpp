#include "interface.h"
#include "yyjson.h"
#include <cstdint>
#include <string>

/**
 * Optimized JSON checksum calculation using yyjson.
 * yyjson is a high-performance C library for parsing JSON that is significantly
 * faster than nlohmann/json due to its efficient memory management and 
 * SIMD-optimized parsing logic.
 */
uint64_t compute_checksum(const std::string& json_input) {
    if (json_input.empty()) return 0;

    // Parse the JSON string into a DOM.
    // Using 0 for flags as the default configuration is highly optimized.
    yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val* root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;

    // Ensure the root is an array as per the task description.
    if (yyjson_is_arr(root)) {
        yyjson_val* item;
        yyjson_arr_iter iter;
        yyjson_arr_iter_init(root, &iter);

        // Iterate through each object in the array.
        while ((item = yyjson_arr_iter_next(&iter))) {
            // Extract "id" field.
            yyjson_val* id_val = yyjson_obj_get(item, "id");
            if (id_val) {
                sum += yyjson_get_uint(id_val);
            }

            // Extract "nested" object.
            yyjson_val* nested_obj = yyjson_obj_get(item, "nested");
            if (nested_obj) {
                // Extract "a" and "b" from the nested object.
                yyjson_val* a_val = yyjson_obj_get(nested_obj, "a");
                if (a_val) {
                    sum += yyjson_get_uint(a_val);
                }
                
                yyjson_val* b_val = yyjson_obj_get(nested_obj, "b");
                if (b_val) {
                    sum += yyjson_get_uint(b_val);
                }
            }
        }
    }

    // Clean up the document memory.
    yyjson_doc_free(doc);

    return sum;
}