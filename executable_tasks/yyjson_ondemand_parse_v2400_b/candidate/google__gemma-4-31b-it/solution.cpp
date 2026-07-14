#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    yyjson_val *item = yyjson_arr_begin(root);
    while (item) {
        // Extract 'id'
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) {
            sum += yyjson_get_uint64(id_val);
        }

        // Extract 'nested' object
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        if (nested && yyjson_obj(nested)) {
            yyjson_val *a_val = yyjson_obj_get(nested, "a");
            if (a_val) sum += yyjson_get_uint64(a_val);

            yyjson_val *b_val = yyjson_obj_get(nested, "b");
            if (b_val) sum += yyjson_get_uint64(b_val);
        }

        item = yyjson_arr_next(item);
    }

    yyjson_doc_free(doc);
    return sum;
}