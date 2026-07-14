#include "interface.h"
#include <yyjson.h>
#include <cstdint>

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;

    yyjson_read_err err;
    // Use NOCOPY flag to avoid copying input buffer for optimal performance
    yyjson_doc* const doc = yyjson_read_opts(json_input.data(), json_input.size(), YYJSON_READ_NOCOPY, nullptr, &err);
    if (!doc) {
        return 0;
    }

    yyjson_val* const root = yyjson_doc_get_root(doc);
    yyjson_arr_iter iter;
    yyjson_val* item;

    // Iterate over each top-level array item
    YYJSON_ARR_FOREACH(root, &iter, item) {
        // Extract and accumulate required fields directly
        sum += yyjson_val_get_uint(yyjson_obj_get(item, "id"));
        const yyjson_val* const nested = yyjson_obj_get(item, "nested");
        if (nested) {
            sum += yyjson_val_get_uint(yyjson_obj_get(nested, "a"));
            sum += yyjson_val_get_uint(yyjson_obj_get(nested, "b"));
        }
    }

    yyjson_doc_free(doc);
    return sum;
}