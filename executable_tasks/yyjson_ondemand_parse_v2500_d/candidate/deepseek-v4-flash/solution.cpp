#include "interface.h"
#include <yyjson.h>
#include <cstdint>
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
    yyjson_val *item, *id_val, *nested_val, *a_val, *b_val;
    yyjson_arr_iter iter = yyjson_arr_iter_with(root);
    while ((item = yyjson_arr_iter_next(&iter))) {
        id_val = yyjson_obj_get(item, "id");
        if (id_val) sum += yyjson_get_uint(id_val);
        nested_val = yyjson_obj_get(item, "nested");
        if (nested_val && yyjson_is_obj(nested_val)) {
            a_val = yyjson_obj_get(nested_val, "a");
            if (a_val) sum += yyjson_get_uint(a_val);
            b_val = yyjson_obj_get(nested_val, "b");
            if (b_val) sum += yyjson_get_uint(b_val);
        }
    }
    yyjson_doc_free(doc);
    return sum;
}