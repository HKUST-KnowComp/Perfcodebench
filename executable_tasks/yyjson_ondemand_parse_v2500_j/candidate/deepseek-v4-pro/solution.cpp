#include "interface.h"
#include <yyjson.h>
#include <vector>
#include <cstring>

uint64_t compute_checksum(const std::string& json_input) {
    size_t len = json_input.size();
    std::vector<char> buf(len + 1);
    std::memcpy(buf.data(), json_input.data(), len);
    buf[len] = '\0';
    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_opts(buf.data(), len, YYJSON_READ_INSITU, nullptr, &err);
    if (!doc) return 0;
    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_arr(root)) {
        yyjson_doc_free(doc);
        return 0;
    }
    uint64_t sum = 0;
    size_t idx, max;
    yyjson_val *item;
    yyjson_arr_foreach(root, idx, max, item) {
        yyjson_val *id_val = yyjson_obj_get(item, "id");
        uint64_t id = yyjson_get_uint(id_val);
        yyjson_val *nested = yyjson_obj_get(item, "nested");
        yyjson_val *a_val = yyjson_obj_get(nested, "a");
        yyjson_val *b_val = yyjson_obj_get(nested, "b");
        uint64_t a = yyjson_get_uint(a_val);
        uint64_t b = yyjson_get_uint(b_val);
        sum += id + a + b;
    }
    yyjson_doc_free(doc);
    return sum;
}