#include "interface.h"
#include "yyjson.h"
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    if (json_input.empty()) {
        return 0;
    }

    // Parse the JSON string using yyjson
    yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) {
        return 0;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!root || !yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    yyjson_val* item;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);

    // Iterate through the array elements
    while ((item = yyjson_arr_iter_next(&iter))) {
        // Check the "flag" field
        yyjson_val* flag_val = yyjson_obj_get(item, "flag");
        if (flag_val && yyjson_get_bool(flag_val)) {
            // Extract "id"
            yyjson_val* id_val = yyjson_obj_get(item, "id");
            if (id_val) {
                sum += yyjson_get_uint(id_val);
            }

            // Extract "value"
            yyjson_val* value_val = yyjson_obj_get(item, "value");
            if (value_val) {
                sum += yyjson_get_uint(value_val);
            }

            // Extract "name" length
            yyjson_val* name_val = yyjson_obj_get(item, "name");
            if (name_val && yyjson_is_str(name_val)) {
                sum += (uint64_t)yyjson_get_len(name_val);
            }
        }
    }

    // Clean up
    yyjson_doc_free(doc);
    return sum;
}
