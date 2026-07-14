#include "interface.h"

#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;

    yyjson_read_err err;
    yyjson_doc* doc = yyjson_read_opts(
        const_cast<char*>(json_input.data()),
        json_input.size(),
        YYJSON_READ_INSITU | YYJSON_READ_STOP_WHEN_DONE,
        nullptr,
        &err);
    if (!doc) return 0; // should not happen under harness

    yyjson_val* root = yyjson_doc_get_root(doc);
    yyjson_arr_iter ai;
    yyjson_arr_iter_init(root, &ai);
    yyjson_val* item;
    while ((item = yyjson_arr_iter_next(&ai))) {
        yyjson_val* id_v   = yyjson_obj_get(item, "id");
        yyjson_val* nest_v = yyjson_obj_get(item, "nested");
        if (yyjson_is_obj(nest_v)) {
            yyjson_val* a_v = yyjson_obj_get(nest_v, "a");
            yyjson_val* b_v = yyjson_obj_get(nest_v, "b");
            sum += yyjson_get_uint(id_v);
            sum += yyjson_get_uint(a_v);
            sum += yyjson_get_uint(b_v);
        }
    }
    yyjson_doc_free(doc);
    return sum;
}