#include "interface.h"
#include "yyjson.h"

#include <cstdint>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;

    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!root || !yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    size_t idx, max;
    yyjson_val *obj;
    yyjson_arr_foreach(root, idx, max, obj) {
        yyjson_val *idv = yyjson_obj_getn(obj, "id", 2);
        yyjson_val *xv = yyjson_obj_getn(obj, "x", 1);
        yyjson_val *vals = yyjson_obj_getn(obj, "vals", 4);

        sum += yyjson_get_uint(idv);
        sum += yyjson_get_uint(xv);

        yyjson_val *v1 = yyjson_arr_get(vals, 1);
        sum += yyjson_get_uint(v1);
    }

    yyjson_doc_free(doc);
    return sum;
}
