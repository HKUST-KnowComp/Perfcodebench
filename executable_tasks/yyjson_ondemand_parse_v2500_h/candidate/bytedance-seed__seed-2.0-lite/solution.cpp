#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) return 0;

    uint64_t sum = 0;
    yyjson_val* root = yyjson_doc_get_root(doc);
    
    if (yyjson_is_arr(root)) {
        const size_t item_count = yyjson_arr_size(root);
        for (size_t i = 0; i < item_count; ++i) {
            yyjson_val* item = yyjson_arr_get(root, i);
            if (!yyjson_is_obj(item)) continue;

            yyjson_val* id_val = yyjson_obj_get(item, "id");
            if (id_val && yyjson_is_uint(id_val)) {
                sum += yyjson_get_uint64(id_val);
            }

            yyjson_val* nested_val = yyjson_obj_get(item, "nested");
            if (nested_val && yyjson_is_obj(nested_val)) {
                yyjson_val* a_val = yyjson_obj_get(nested_val, "a");
                if (a_val && yyjson_is_uint(a_val)) {
                    sum += yyjson_get_uint64(a_val);
                }
                yyjson_val* b_val = yyjson_obj_get(nested_val, "b");
                if (b_val && yyjson_is_uint(b_val)) {
                    sum += yyjson_get_uint64(b_val);
                }
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}