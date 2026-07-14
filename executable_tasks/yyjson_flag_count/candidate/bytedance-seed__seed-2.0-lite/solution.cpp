#include "interface.h"
#include <yyjson.h>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_read_err err;
    yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG, &err);
    if (!doc) return 0;

    uint64_t sum = 0;
    yyjson_val* root = yyjson_doc_get_root(doc);
    const size_t arr_len = yyjson_arr_size(root);

    for (size_t i = 0; i < arr_len; ++i) {
        yyjson_val* item = yyjson_arr_get(root, i);
        yyjson_val* flag_val = yyjson_obj_get(item, "flag");
        if (!yyjson_is_true(flag_val)) continue;

        sum += yyjson_get_uint(yyjson_obj_get(item, "id"));
        sum += yyjson_get_uint(yyjson_obj_get(item, "value"));

        size_t name_len;
        yyjson_get_strn(yyjson_obj_get(item, "name"), &name_len);
        sum += static_cast<uint64_t>(name_len);
    }

    yyjson_doc_free(doc);
    return sum;
}