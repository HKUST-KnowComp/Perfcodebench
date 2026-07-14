#include "interface.h"
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) return 0;

    yyjson_val* root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;

    yyjson_arr_iter it;
    yyjson_arr_iter_init(root, &it);
    yyjson_val* item;
    while ((item = yyjson_arr_iter_next(&it))) {
        yyjson_val* nested = yyjson_obj_getn(item, "nested", 6);
        sum += yyjson_get_uint(yyjson_obj_getn(item, "id", 2));
        sum += yyjson_get_uint(yyjson_obj_getn(nested, "a", 1));
        sum += yyjson_get_uint(yyjson_obj_getn(nested, "b", 1));
    }

    yyjson_doc_free(doc);
    return sum;
}
