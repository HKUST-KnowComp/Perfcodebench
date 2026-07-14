#include "interface.h"
#include <yyjson.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    // Mutable copy for insitu parsing (zero-copy string handling)
    std::string buffer(json_input);
    yyjson_doc *doc = yyjson_read_insitu(buffer.data(), buffer.size(), 0);
    if (!doc) {
        return 0;
    }
    yyjson_val *root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    if (yyjson_is_arr(root)) {
        yyjson_val *item;
        yyjson_arr_iter iter = yyjson_arr_iter_with(root);
        while ((item = yyjson_arr_iter_next(&iter))) {
            sum += yyjson_get_uint(yyjson_obj_get(item, "id"));
            yyjson_val *nested = yyjson_obj_get(item, "nested");
            sum += yyjson_get_uint(yyjson_obj_get(nested, "a"));
            sum += yyjson_get_uint(yyjson_obj_get(nested, "b"));
        }
    }
    yyjson_doc_free(doc);
    return sum;
}