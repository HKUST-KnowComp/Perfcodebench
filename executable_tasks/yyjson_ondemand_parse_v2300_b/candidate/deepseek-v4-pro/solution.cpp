#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) return 0;
    yyjson_val *root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);
    yyjson_val *item;
    while ((item = yyjson_arr_iter_next(&iter))) {
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) sum += yyjson_get_uint(id_val);
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        if (nested) {
            yyjson_val *a = yyjson_obj_get(nested, "a");
            if (a) sum += yyjson_get_uint(a);
            yyjson_val *b = yyjson_obj_get(nested, "b");
            if (b) sum += yyjson_get_uint(b);
        }
    }
    yyjson_doc_free(doc);
    return sum;
}