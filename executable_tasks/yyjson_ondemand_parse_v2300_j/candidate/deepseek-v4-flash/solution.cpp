#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    uint64_t sum = 0;
    if (doc) {
        yyjson_val *root = yyjson_doc_get_root(doc);
        if (yyjson_val_is_arr(root)) {
            size_t idx, max;
            yyjson_val *val;
            yyjson_arr_foreach(root, idx, max, val) {
                yyjson_val *id_val = yyjson_obj_get(val, "id");
                sum += yyjson_get_uint64(id_val);
                yyjson_val *nested_val = yyjson_obj_get(val, "nested");
                yyjson_val *a_val = yyjson_obj_get(nested_val, "a");
                yyjson_val *b_val = yyjson_obj_get(nested_val, "b");
                sum += yyjson_get_uint64(a_val);
                sum += yyjson_get_uint64(b_val);
            }
        }
        yyjson_doc_free(doc);
    }
    return sum;
}
