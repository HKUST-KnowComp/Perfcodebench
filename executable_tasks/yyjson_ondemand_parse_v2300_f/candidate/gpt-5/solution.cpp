#include "interface.h"

#include <cstdint>
#include <string>
#include "yyjson.h"

static inline bool get_nonneg_u64(yyjson_val* v, uint64_t& out) {
    if (!v) return false;
    if (yyjson_is_uint(v)) {
        out = yyjson_get_uint(v);
        return true;
    }
    if (yyjson_is_sint(v)) {
        int64_t s = yyjson_get_sint(v);
        if (s >= 0) {
            out = static_cast<uint64_t>(s);
            return true;
        }
    }
    return false;
}

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;

    if (json_input.empty()) return 0;

    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) return 0; // invalid JSON: return 0 (inputs are expected valid in benchmarks)

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (yyjson_is_arr(root)) {
        yyjson_arr_iter arr_it;
        yyjson_arr_iter_init(root, &arr_it);
        yyjson_val* obj;
        while ((obj = yyjson_arr_iter_next(&arr_it))) {
            if (!yyjson_is_obj(obj)) continue;

            // id
            uint64_t v;
            if (get_nonneg_u64(yyjson_obj_get(obj, "id"), v)) sum += v;

            // nested.a and nested.b
            yyjson_val* nested = yyjson_obj_get(obj, "nested");
            if (nested && yyjson_is_obj(nested)) {
                if (get_nonneg_u64(yyjson_obj_get(nested, "a"), v)) sum += v;
                if (get_nonneg_u64(yyjson_obj_get(nested, "b"), v)) sum += v;
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}
