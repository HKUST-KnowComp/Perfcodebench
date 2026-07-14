#include "interface.h"
#include <yyjson.h>
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) return 0;
    yyjson_val *root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);
    yyjson_val *item;
    while ((item = yyjson_arr_iter_next(&iter))) {
        yyjson_val *flag = yyjson_obj_get(item, "flag");
        if (!yyjson_is_true(flag)) continue;
        sum += yyjson_get_uint(yyjson_obj_get(item, "id"));
        sum += yyjson_get_len(yyjson_obj_get(item, "tag"));
        sum += yyjson_get_uint(yyjson_obj_get(item, "value"));
    }
    yyjson_doc_free(doc);
    return sum;
}
