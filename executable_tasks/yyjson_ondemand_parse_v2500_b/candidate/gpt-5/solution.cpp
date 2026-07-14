#include "interface.h"
#include "yyjson.h"

#include <cstddef>

uint64_t compute_checksum(const std::string& json_input) {
    // Parse JSON using yyjson (immutable input, no in-situ modification)
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) {
        return 0; // Assuming valid input per harness; return 0 on failure as a safe fallback
    }

    uint64_t sum = 0;
    yyjson_val *root = yyjson_doc_get_root(doc);

    if (yyjson_is_arr(root)) {
        size_t idx;
        yyjson_val *obj;
        yyjson_arr_foreach(root, idx, obj) {
            // Fetch id
            yyjson_val *idv = yyjson_obj_getn(obj, "id", 2);
            if (idv) sum += yyjson_get_uint(idv);

            // Fetch nested object
            yyjson_val *nested = yyjson_obj_getn(obj, "nested", 6);
            if (nested) {
                yyjson_val *av = yyjson_obj_getn(nested, "a", 1);
                if (av) sum += yyjson_get_uint(av);
                yyjson_val *bv = yyjson_obj_getn(nested, "b", 1);
                if (bv) sum += yyjson_get_uint(bv);
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}
