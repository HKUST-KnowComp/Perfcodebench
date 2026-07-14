#include "interface.h"
#include "yyjson.h"
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    if (json_input.empty()) return 0;

    // Read JSON input using yyjson
    yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    yyjson_val *item;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);

    // Iterate through the array of objects
    while ((item = yyjson_arr_iter_next(&iter))) {
        // Extract "id"
        yyjson_val* id_val = yyjson_obj_get(item, "id");
        if (id_val) {
            sum += yyjson_get_uint(id_val);
        }

        // Extract "nested" object
        yyjson_val* nested_obj = yyjson_obj_get(item, "nested");
        if (nested_obj) {
            // Extract "nested.a"
            yyjson_val* a_val = yyjson_obj_get(nested_obj, "a");
            if (a_val) {
                sum += yyjson_get_uint(a_val);
            }
            // Extract "nested.b"
            yyjson_val* b_val = yyjson_obj_get(nested_obj, "b");
            if (b_val) {
                sum += yyjson_get_uint(b_val);
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}