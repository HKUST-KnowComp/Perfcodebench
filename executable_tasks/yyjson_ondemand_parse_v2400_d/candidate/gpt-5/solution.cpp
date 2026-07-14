#include "interface.h"

#include <yyjson.h>
#include <cstdint>
#include <cstddef>

static inline uint64_t to_u64(yyjson_val* v) {
    if (!v) return 0u;
    if (yyjson_is_uint(v)) return yyjson_get_uint(v);
    if (yyjson_is_sint(v)) {
        int64_t s = yyjson_get_sint(v);
        return s >= 0 ? static_cast<uint64_t>(s) : 0u;
    }
    if (yyjson_is_real(v)) {
        double d = yyjson_get_real(v);
        if (d <= 0.0) return 0u;
        return static_cast<uint64_t>(d);
    }
    return 0u;
}

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) return 0u;

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0u;
    }

    uint64_t sum = 0u;

    size_t idx, max;
    yyjson_val* obj;
    yyjson_arr_foreach(root, idx, max, obj) {
        if (!yyjson_is_obj(obj)) continue;

        yyjson_val* id = yyjson_obj_getn(obj, "id", 2);
        sum += to_u64(id);

        yyjson_val* nested = yyjson_obj_getn(obj, "nested", 6);
        if (nested && yyjson_is_obj(nested)) {
            yyjson_val* a = yyjson_obj_getn(nested, "a", 1);
            yyjson_val* b = yyjson_obj_getn(nested, "b", 1);
            sum += to_u64(a);
            sum += to_u64(b);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}
