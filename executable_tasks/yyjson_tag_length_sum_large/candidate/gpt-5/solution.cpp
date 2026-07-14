#include "interface.h"

#include <yyjson.h>

#include <string>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_read_err err;
    yyjson_doc* doc = yyjson_read_opts(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG, nullptr, &err);
    if (!doc) {
        return 0;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;

    if (yyjson_is_arr(root)) {
        yyjson_val* item;
        size_t idx, max;
        yyjson_arr_foreach(root, idx, max, item) {
            if (!yyjson_is_obj(item)) {
                continue;
            }

            yyjson_val* flag = yyjson_obj_getn(item, "flag", 4);
            if (!(flag && yyjson_get_bool(flag))) {
                continue;
            }

            yyjson_val* id = yyjson_obj_getn(item, "id", 2);
            yyjson_val* tag = yyjson_obj_getn(item, "tag", 3);
            yyjson_val* value = yyjson_obj_getn(item, "value", 5);

            if (id) {
                sum += yyjson_get_uint(id);
            }
            if (tag && yyjson_is_str(tag)) {
                sum += static_cast<uint64_t>(yyjson_get_len(tag));
            }
            if (value) {
                sum += yyjson_get_uint(value);
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}
