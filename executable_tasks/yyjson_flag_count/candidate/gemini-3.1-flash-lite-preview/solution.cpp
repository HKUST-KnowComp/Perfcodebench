#include "interface.h"
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    uint64_t sum = 0;
    yyjson_val* root = yyjson_doc_get_root(doc);

    if (yyjson_is_arr(root)) {
        size_t idx, max;
        yyjson_val* item;
        yyjson_arr_foreach(root, idx, max, item) {
            yyjson_val* flag = yyjson_obj_get(item, "flag");
            if (flag && yyjson_get_bool(flag)) {
                yyjson_val* id = yyjson_obj_get(item, "id");
                yyjson_val* val = yyjson_obj_get(item, "value");
                yyjson_val* name = yyjson_obj_get(item, "name");

                if (id) sum += yyjson_get_uint(id);
                if (val) sum += yyjson_get_uint(val);
                if (name && yyjson_is_str(name)) {
                    sum += yyjson_get_len(name);
                }
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}