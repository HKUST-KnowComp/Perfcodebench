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

        // Extract 'id'
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val && yyjson_is_u64(id_val)) {
            sum += yyjson_u64(id_val);
        }

        // Extract 'nested.a' and 'nested.b'
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        if (nested && yyjson_is_obj(nested)) {
            yyjson_val *a_val = yyjson_obj_get(nested, "a");
            if (a_val && yyjson_is_u64(a_val)) {
                sum += yyjson_u64(a_val);
            }

            yyjson_val *b_val = yyjson_obj_get(nested, "b");
            if (b_val && yyjson_is_u64(b_val)) {
                sum += yyjson_u64(b_val);
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}