#include "interface.h"
#include <yyjson.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    size_t len = yyjson_arr_len(root);
    for (size_t i = 0; i < len; ++i) {
        yyjson_val *item = yyjson_arr_get(root, i);
        if (!yyjson_is_obj(item)) continue;

        // Extract "id"
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) sum += (uint64_t)yyjson_get_uint(id_val);

        // Extract "x"
        yyjson_val *x_val = yyjson_obj_get(item, "x");
        if (x_val) sum += (uint64_t)yyjson_get_uint(x_val);

        // Extract "vals"[1]
        yyjson_val *vals_val = yyjson_obj_get(item, "vals");
        if (vals_val && yyjson_is_arr(vals_val)) {
            yyjson_val *v1 = yyjson_arr_get(vals_val, 1);
            if (v1) sum += (uint64_t)yyjson_get_uint(v1);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}