#include "interface.h"
#include "yyjson.h"
#include <cstring>

uint64_t compute_checksum(const std::string& json_input) {
    if (json_input.empty()) return 0;

    // Use mutable copy for in-situ parsing to maximize performance
    char* buf = const_cast<char*>(json_input.c_str());
    size_t len = json_input.size();

    yyjson_doc* doc = yyjson_read_opts(buf, len, YYJSON_READ_ALLOW_TRAILING_COMMAS | YYJSON_READ_INSITU, nullptr, nullptr);
    if (!doc) return 0;

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }

    uint64_t sum = 0;
    yyjson_val* item;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(root, &iter);

    while ((item = yyjson_arr_iter_next(&iter))) {
        // Extract "id"
        yyjson_val* id_val = yyjson_obj_get(item, "id");
        if (id_val) {
            sum += yyjson_get_uint(id_val);
        }

        // Extract "nested"
        yyjson_val* nested_obj = yyjson_obj_get(item, "nested");
        if (nested_obj) {
            // Extract "nested" -> "a"
            yyjson_val* a_val = yyjson_obj_get(nested_obj, "a");
            if (a_val) {
                sum += yyjson_get_uint(a_val);
            }
            // Extract "nested" -> "b"
            yyjson_val* b_val = yyjson_obj_get(nested_obj, "b");
            if (b_val) {
                sum += yyjson_get_uint(b_val);
            }
        }
    }

    yyjson_doc_free(doc);
    return sum;
}