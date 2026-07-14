#include "interface.h"

#include <yyjson.h>
#include <cstring>
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    // yyjson in-situ parsing requires a mutable buffer
    // Copy input to a mutable buffer (yyjson may modify it)
    size_t len = json_input.size();
    char* buf = static_cast<char*>(malloc(len + YYJSON_PADDING_SIZE));
    if (!buf) return 0;
    memcpy(buf, json_input.data(), len);
    memset(buf + len, 0, YYJSON_PADDING_SIZE);

    yyjson_doc* doc = yyjson_read_opts(buf, len, YYJSON_READ_INSITU, nullptr, nullptr);
    if (!doc) {
        free(buf);
        return 0;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);
    uint64_t sum = 0;

    size_t idx, max;
    yyjson_val* item;
    yyjson_arr_foreach(root, idx, max, item) {
        yyjson_val* flag_val = yyjson_obj_get(item, "flag");
        if (!flag_val || !yyjson_get_bool(flag_val)) {
            continue;
        }

        yyjson_val* id_val = yyjson_obj_get(item, "id");
        yyjson_val* value_val = yyjson_obj_get(item, "value");
        yyjson_val* name_val = yyjson_obj_get(item, "name");

        if (id_val) {
            sum += yyjson_get_uint(id_val);
        }
        if (value_val) {
            sum += yyjson_get_uint(value_val);
        }
        if (name_val) {
            sum += static_cast<uint64_t>(yyjson_get_len(name_val));
        }
    }

    yyjson_doc_free(doc);
    free(buf);
    return sum;
}
