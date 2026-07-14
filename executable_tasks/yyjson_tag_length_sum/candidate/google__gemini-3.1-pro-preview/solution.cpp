#include "interface.h"
#include "yyjson.h"
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    if (json_input.empty()) return 0;

    // Use yyjson for high-performance parsing
    yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    size_t idx, max;
    yyjson_val *item;

    // Iterate through the array
    yyjson_arr_foreach(root, idx, max, item) {
        if (!yyjson_is_obj(item)) continue;

        // Check "flag" first to skip items early
        yyjson_val *flag_val = yyjson_obj_get(item, "flag");
        if (!flag_val || !yyjson_get_bool(flag_val)) {
            continue;
        }

        // Extract "id"
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) {
            sum += yyjson_get_uint(id_val);
        }

        // Extract "tag" length without allocating a std::string
        yyjson_val *tag_val = yyjson_obj_get(item, "tag");
        if (tag_val) {
            sum += (uint64_t)yyjson_get_len(tag_val);
        }

        // Extract "value"
        yyjson_val *value_val = yyjson_obj_get(item, "value");
        if (value_val) {
            sum += yyjson_get_uint(value_val);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}