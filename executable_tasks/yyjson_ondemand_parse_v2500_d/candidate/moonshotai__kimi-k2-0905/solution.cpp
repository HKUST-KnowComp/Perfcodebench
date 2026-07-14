#include "interface.h"

#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_read_err err;
    yyjson_doc* doc = yyjson_read_opts(
        const_cast<char*>(json_input.data()),
        json_input.size(),
        YYJSON_READ_NOFLAG,
        nullptr,   // alc
        &err);
    if (!doc) return 0;

    yyjson_val* root = yyjson_doc_get_root(doc);
    yyjson_arr_iter ai;
    yyjson_arr_iter_init(root, &ai);

    uint64_t sum = 0;
    yyjson_val* item;
    while ((item = yyjson_arr_iter_next(&ai))) {
        yyjson_val* id   = yyjson_obj_get(item, "id");
        yyjson_val* nested = yyjson_obj_get(item, "nested");
        yyjson_val* a   = yyjson_obj_get(nested, "a");
        yyjson_val* b   = yyjson_obj_get(nested, "b");

        sum += yyjson_get_uint(id);
        sum += yyjson_get_uint(a);
        sum += yyjson_get_uint(b);
    }

    yyjson_doc_free(doc);
    return sum;
}