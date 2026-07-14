#include "interface.h"
#include <yyjson.h>
#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) {
        return 0;
    }
    yyjson_val* root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;
    yyjson_val* item;
    yyjson_val* val;
    size_t idx, max;
    yyjson_arr_foreach(root, idx, max, item) {
        val = yyjson_obj_get(item, "flag");
        if (!val || !yyjson_is_bool(val) || !yyjson_get_bool(val)) {
            continue;
        }
        val = yyjson_obj_get(item, "id");
        if (!val || !yyjson_is_uint(val)) {
            continue;
        }
        sum += yyjson_get_uint(val);
        val = yyjson_obj_get(item, "tag");
        if (!val || !yyjson_is_str(val)) {
            continue;
        }
        sum += static_cast<uint64_t>(yyjson_val_len(val));
        val = yyjson_obj_get(item, "value");
        if (!val || !yyjson_is_uint(val)) {
            continue;
        }
        sum += yyjson_get_uint(val);
    }
    yyjson_doc_free(doc);
    return sum;
}