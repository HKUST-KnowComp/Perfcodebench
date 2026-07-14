#include "interface.h"
#include "yyjson.h"
#include <string>

uint64_t compute_checksum(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read_capture(json_input.c_str(), json_input.size(), 0);
    if (!doc) return 0;

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    yyjson_val *item = root;
    while ((item = yyjson_arr_next(item))) {
        // Check flag first to skip unnecessary lookups
        yyjson_val *flag_val = yyjson_obj_get(item, "flag");
        if (!flag_val || !yyjson_get_bool(flag_val)) {
            continue;
        }

        // Process id
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        if (id_val) {
            sum += (uint64_t)yyjson_get_int(id_val);
        }

        // Process value
        yyjson_val *val_val = yyjson_obj_get(item, "value");
        if (val_val) {
            sum += (uint64_t)yyjson_get_int(val_val);
        }

        // Process name length
        yyjson_val *name_val = yyjson_obj_get(item, "name");
        if (name_val) {
            const char *name_str = yyjson_get_str(name_val);
            if (name_str) {
                sum += (uint64_t)strlen(name_str);
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}