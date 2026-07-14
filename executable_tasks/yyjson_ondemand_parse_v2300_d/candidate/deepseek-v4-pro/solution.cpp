#include "interface.h"

#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    yyjson_val *root = yyjson_doc_get_root(doc);
    size_t len = yyjson_arr_size(root);
    uint64_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        yyjson_val *item = yyjson_arr_get(root, i);
        yyjson_val *id_val = yyjson_obj_getn(item, "id", 2);
        sum += yyjson_get_uint(id_val);
        yyjson_val *nested = yyjson_obj_getn(item, "nested", 6);
        yyjson_val *a_val = yyjson_obj_getn(nested, "a", 1);
        yyjson_val *b_val = yyjson_obj_getn(nested, "b", 1);
        sum += yyjson_get_uint(a_val);
        sum += yyjson_get_uint(b_val);
    }
    yyjson_doc_free(doc);
    return sum;
}