#include "interface.h"
#include "yyjson.h"

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
        if (yyjson_obj(item)) {
            yyjson_val *val_id = yyjson_obj_get(item, "id");
            if (val_id) sum += (uint64_t)yyjson_get_int(val_id);

            yyjson_val *val_nested = yyjson_obj_get(item, "nested");
            if (val_nested && yyjson_obj(val_nested)) {
                yyjson_val *val_a = yyjson_obj_get(val_nested, "a");
                if (val_a) sum += (uint64_t)yyjson_get_int(val_a);
                
                yyjson_val *val_b = yyjson_obj_get(val_nested, "b");
                if (val_b) sum += (uint64_t)yyjson_get_int(val_b);
            }
        }
        item = yyjson_arr_next(item);
    }

    yyjson_doc_free(doc);
    return sum;
}