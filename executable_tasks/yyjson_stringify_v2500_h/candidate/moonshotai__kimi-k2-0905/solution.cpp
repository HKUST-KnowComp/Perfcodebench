#include "interface.h"
#include "yyjson.h"
#include <string>
#include <vector>

std::string build_json(const std::vector<uint32_t>& ids,
                       const std::vector<uint32_t>& values,
                       const std::vector<std::string>& names) {
    const size_t n = ids.size();

    yyjson_doc* doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val* root = yyjson_mut_arr(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_arr_reserve(doc, root, n);

    for (size_t i = 0; i < n; ++i) {
        yyjson_mut_val* obj = yyjson_mut_obj(doc);
        yyjson_mut_obj_reserve(doc, obj, 4);

        yyjson_mut_obj_add_uint(doc, obj, "id", ids[i]);
        yyjson_mut_obj_add_bool(doc, obj, "flag", (ids[i] % 3U) == 0U);
        yyjson_mut_obj_add_uint(doc, obj, "value", values[i]);
        yyjson_mut_obj_add_strn(doc, obj, "name",
                                names[i].data(), names[i].size());

        yyjson_mut_arr_append(root, obj);
    }

    size_t json_len = 0;
    const char* json = yyjson_mut_write(doc, YYJSON_WRITE_NOFLAG, &json_len);
    std::string result(json, json_len);

    free((void*)json);
    yyjson_mut_doc_free(doc);
    return result;
}