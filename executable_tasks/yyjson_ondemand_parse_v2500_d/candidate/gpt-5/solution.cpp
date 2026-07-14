#include "interface.h"
#include "yyjson.h"

#include <cstdint>
#include <string>

static inline uint64_t get_u64_from_val(yyjson_val *v) {
    if (!v) return 0;
    if (yyjson_is_uint(v)) return (uint64_t)yyjson_get_uint(v);
    if (yyjson_is_sint(v)) return (uint64_t)yyjson_get_sint(v);
    if (yyjson_is_real(v)) return (uint64_t)yyjson_get_real(v);
    return 0;
}

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;
    if (json_input.empty()) return 0;

    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!root || !yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    yyjson_val *item;
    size_t idx, max;
    yyjson_arr_foreach(root, idx, max, item) {
        if (!item || !yyjson_is_obj(item)) continue;

        // Use object iterator to minimize repeated scans
        yyjson_obj_iter it;
        yyjson_obj_iter_init(item, &it);

        yyjson_val *idv = yyjson_obj_iter_get(&it, "id");
        if (idv) sum += get_u64_from_val(idv);

        yyjson_val *nested = yyjson_obj_iter_get(&it, "nested");
        if (nested && yyjson_is_obj(nested)) {
            yyjson_obj_iter it2;
            yyjson_obj_iter_init(nested, &it2);

            yyjson_val *av = yyjson_obj_iter_get(&it2, "a");
            if (av) sum += get_u64_from_val(av);

            yyjson_val *bv = yyjson_obj_iter_get(&it2, "b");
            if (bv) sum += get_u64_from_val(bv);
        }
    }

    yyjson_doc_free(doc);
    return sum;
}
