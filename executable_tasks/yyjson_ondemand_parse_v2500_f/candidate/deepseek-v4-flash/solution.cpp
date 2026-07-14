#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) {
        throw std::runtime_error("Failed to parse JSON");
    }
    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        throw std::runtime_error("Root is not an array");
    }
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);
    yyjson_val* item;
    uint64_t sum = 0;
    while ((item = yyjson_arr_iter_next(&iter)) != NULL) {
        if (!yyjson_is_obj(item)) continue;
        yyjson_val* id_val = yyjson_obj_get(item, "id");
        if (id_val) sum += yyjson_get_uint(id_val);
        yyjson_val* nested_val = yyjson_obj_get(item, "nested");
        if (nested_val && yyjson_is_obj(nested_val)) {
            yyjson_val* a_val = yyjson_obj_get(nested_val, "a");
            if (a_val) sum += yyjson_get_uint(a_val);
            yyjson_val* b_val = yyjson_obj_get(nested_val, "b");
            if (b_val) sum += yyjson_get_uint(b_val);
        }
    }
    yyjson_doc_free(doc);
    return sum;
}
