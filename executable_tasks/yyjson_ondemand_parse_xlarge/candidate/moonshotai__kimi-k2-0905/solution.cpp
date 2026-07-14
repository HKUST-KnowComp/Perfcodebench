#include "interface.h"
#include "yyjson.h"
#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;

    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_opts(const_cast<char*>(json_input.data()),
                                        json_input.size(),
                                        YYJSON_READ_INSITU | YYJSON_READ_STOP_WHEN_DONE,
                                        nullptr, &err);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);

    yyjson_val *obj;
    while ((obj = yyjson_arr_iter_next(&iter))) {
        yyjson_val *id   = yyjson_obj_getn(obj, "id", 2);
        yyjson_val *nested = yyjson_obj_getn(obj, "nested", 6);
        if (yyjson_is_obj(nested)) {
            yyjson_val *a = yyjson_obj_getn(nested, "a", 1);
            yyjson_val *b = yyjson_obj_getn(nested, "b", 1);
            sum += yyjson_get_uint(id);
            sum += yyjson_get_uint(a);
            sum += yyjson_get_uint(b);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}