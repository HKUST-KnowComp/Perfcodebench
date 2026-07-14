#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_read_err err;
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0, &err);
    if (!doc) return 0;

    uint64_t sum = 0;
    yyjson_val* root = yyjson_doc_get_root(doc);
    const size_t arr_len = yyjson_arr_size(root);

    for (size_t i = 0; i < arr_len; i++) {
        yyjson_val* item = yyjson_arr_get(root, i);
        sum += yyjson_get_uint64(yyjson_obj_get(item, "id"));
        sum += yyjson_get_uint64(yyjson_obj_get(item, "x"));
        yyjson_val* vals_arr = yyjson_obj_get(item, "vals");
        sum += yyjson_get_uint64(yyjson_arr_get(vals_arr, 1));
    }

    yyjson_doc_free(doc);
    return sum;
}