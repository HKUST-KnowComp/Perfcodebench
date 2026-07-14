#include "interface.h"
#include "yyjson.h"

#include <cstdint>
#include <string>

static inline uint64_t to_u64(yyjson_val *v) {
    if (!v) return 0;
    if (yyjson_is_uint(v)) return yyjson_get_uint(v);
    if (yyjson_is_sint(v)) {
        int64_t s = yyjson_get_sint(v);
        return s >= 0 ? (uint64_t)s : 0u;
    }
    if (yyjson_is_real(v)) {
        double d = yyjson_get_real(v);
        return d >= 0.0 ? (uint64_t)d : 0u;
    }
    return 0u;
}

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;

    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!root || !yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    yyjson_arr_iter arr_iter;
    yyjson_arr_iter_init(root, &arr_iter);
    yyjson_val *item;

    while ((item = yyjson_arr_iter_next(&arr_iter))) {
        if (!yyjson_is_obj(item)) continue;

        yyjson_val *id = yyjson_obj_get(item, "id");
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        if (!nested || !yyjson_is_obj(nested)) continue;

        yyjson_val *a = yyjson_obj_get(nested, "a");
        yyjson_val *b = yyjson_obj_get(nested, "b");

        sum += to_u64(id);
        sum += to_u64(a);
        sum += to_u64(b);
    }

    yyjson_doc_free(doc);
    return sum;
}
