#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    // zero-copy read, no validation needed (input guaranteed valid)
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;

    yyjson_val *item;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);
    while ((item = yyjson_arr_iter_next(&iter))) {
        sum += yyjson_get_uint64(yyjson_obj_get(item, "id"));
        sum += yyjson_get_uint64(yyjson_obj_get(item, "x"));
        yyjson_val *vals = yyjson_obj_get(item, "vals");
        sum += yyjson_get_uint64(yyjson_arr_get(vals, 1));
    }

    yyjson_doc_free(doc);
    return sum;
}