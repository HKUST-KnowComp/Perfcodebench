#include "interface.h"

#include <cstdint>
#include <string>
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);
    uint64_t sum = 0;
    yyjson_val *item;
    while ((item = yyjson_arr_iter_next(&iter))) {
        // Get "id"
        yyjson_val *id_val = yyjson_obj_getn(item, "id", 2);
        sum += yyjson_get_uint(id_val);
        // Get nested object and its fields
        yyjson_val *nested = yyjson_obj_getn(item, "nested", 6);
        yyjson_val *a_val = yyjson_obj_getn(nested, "a", 1);
        sum += yyjson_get_uint(a_val);
        yyjson_val *b_val = yyjson_obj_getn(nested, "b", 1);
        sum += yyjson_get_uint(b_val);
    }
    yyjson_doc_free(doc);
    return sum;
}