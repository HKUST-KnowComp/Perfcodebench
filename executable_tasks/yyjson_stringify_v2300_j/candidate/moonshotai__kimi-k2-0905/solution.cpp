#include "interface.h"
#include "yyjson.h"
#include <string>
#include <vector>

std::string build_json(const std::vector<uint32_t>& ids,
                       const std::vector<uint32_t>& values,
                       const std::vector<std::string>& names) {
    const size_t n = ids.size();

    // Pre-size output buffer generously (empirically ~40 bytes per record)
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_doc_set_str_pool_size(doc, n * 64);
    yyjson_mut_doc_set_imut_val_pool_size(doc, n * 8);

    yyjson_mut_val* root = yyjson_mut_arr(doc);
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

    yyjson_mut_doc_set_root(doc, root);

    // Minified JSON, no extra whitespace
    yyjson_write_err err;
    size_t json_len = 0;
    char* json = yyjson_mut_write_opts(doc,
                                       YYJSON_WRITE_NOFLAG,
                                       NULL,  // alc
                                       &json_len,
                                       &err);

    std::string result(json, json_len);
    free(json);
    yyjson_mut_doc_free(doc);
    return result;
}