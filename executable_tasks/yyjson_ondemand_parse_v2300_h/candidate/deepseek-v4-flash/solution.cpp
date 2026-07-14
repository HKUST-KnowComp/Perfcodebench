#include "interface.h"
#include <yyjson.h>
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    // Parse the JSON string to a yyjson document.
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) {
        return 0; // Should not happen with valid input.
    }

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    size_t idx, max;
    yyjson_val *item;
    yyjson_arr_foreach(root, idx, max, item) {
        // Extract "id"
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) {
            sum += yyjson_get_uint(id_val);
        }

        // Extract nested object
        yyjson_val *nested_val = yyjson_obj_get(item, "nested");
        if (nested_val && yyjson_is_obj(nested_val)) {
            yyjson_val *a_val = yyjson_obj_get(nested_val, "a");
            if (a_val) {
                sum += yyjson_get_uint(a_val);
            }
            yyjson_val *b_val = yyjson_obj_get(nested_val, "b");
            if (b_val) {
                sum += yyjson_get_uint(b_val);
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}
