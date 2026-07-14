#include "interface.h"

#include <yyjson.h>
#include <cstdint>
#include <string>

static inline uint64_t get_u64_safe(yyjson_val *v) {
    if (!v) return 0u;
    if (yyjson_is_uint(v)) {
        return yyjson_get_uint(v);
    }
    if (yyjson_is_sint(v)) {
        int64_t s = yyjson_get_sint(v);
        return s >= 0 ? static_cast<uint64_t>(s) : 0u;
    }
    return 0u;
}

uint64_t compute_checksum(const std::string& json_input) {
    if (json_input.empty()) return 0u;

    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) return 0u;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0u;
    }

    uint64_t sum = 0u;

    size_t idx, max;
    yyjson_val *item;
    yyjson_arr_foreach(root, idx, max, item) {
        if (!yyjson_is_obj(item)) continue;

        yyjson_val *id = yyjson_obj_getn(item, "id", 2);
        sum += get_u64_safe(id);

        yyjson_val *nested = yyjson_obj_getn(item, "nested", 6);
        if (nested && yyjson_is_obj(nested)) {
            yyjson_val *a = yyjson_obj_getn(nested, "a", 1);
            sum += get_u64_safe(a);
            yyjson_val *b = yyjson_obj_getn(nested, "b", 1);
            sum += get_u64_safe(b);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}
