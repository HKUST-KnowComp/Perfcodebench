#include "interface.h"

#include <yyjson.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_read_err err;
    yyjson_doc* doc = yyjson_read_opts(
        const_cast<char*>(json_input.data()),
        json_input.size(),
        YYJSON_READ_INSITU | YYJSON_READ_STOP_WHEN_DONE,
        nullptr, &err);
    if (!doc) return 0;

    uint64_t sum = 0;
    yyjson_val* arr = yyjson_doc_get_root(doc);
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(arr, &iter);
    yyjson_val* item;
    while ((item = yyjson_arr_iter_next(&iter))) {
        yyjson_val* flag = yyjson_obj_get(item, "flag");
        if (!yyjson_get_bool(flag)) continue;

        yyjson_val* id   = yyjson_obj_get(item, "id");
        yyjson_val* tag  = yyjson_obj_get(item, "tag");
        yyjson_val* value = yyjson_obj_get(item, "value");

        sum += yyjson_get_uint(id);
        sum += yyjson_get_len(tag);
        sum += yyjson_get_uint(value);
    }
    yyjson_doc_free(doc);
    return sum;
}