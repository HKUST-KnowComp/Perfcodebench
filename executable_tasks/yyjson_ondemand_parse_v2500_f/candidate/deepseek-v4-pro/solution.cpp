#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    yyjson_val *root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(&iter, root);
    yyjson_val *item;
    while ((item = yyjson_arr_iter_next(&iter))) {
        sum += yyjson_get_uint(yyjson_obj_get(item, "id"));
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        sum += yyjson_get_uint(yyjson_obj_get(nested, "a"));
        sum += yyjson_get_uint(yyjson_obj_get(nested, "b"));
    }
    yyjson_doc_free(doc);
    return sum;
}