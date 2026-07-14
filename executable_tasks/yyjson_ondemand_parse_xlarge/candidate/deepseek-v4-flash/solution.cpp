#include <cstdint>
#include <string>
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;  // Fallback, but input assumed valid

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    yyjson_val *item;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);

    while ((item = yyjson_arr_iter_next(&iter))) {
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) sum += (uint64_t)yyjson_get_uint(id_val);

        yyjson_val *nested = yyjson_obj_get(item, "nested");
        if (nested && yyjson_is_obj(nested)) {
            yyjson_val *a_val = yyjson_obj_get(nested, "a");
            if (a_val) sum += (uint64_t)yyjson_get_uint(a_val);

            yyjson_val *b_val = yyjson_obj_get(nested, "b");
            if (b_val) sum += (uint64_t)yyjson_get_uint(b_val);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}
