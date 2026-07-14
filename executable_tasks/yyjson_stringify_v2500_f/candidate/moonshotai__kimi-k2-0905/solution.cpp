#include "interface.h"
#include <yyjson.h>

std::string build_json(const std::vector<uint32_t>& ids,
                       const std::vector<uint32_t>& values,
                       const std::vector<std::string>& names) {
    size_t n = ids.size();
    yyjson_doc* doc   = yyjson_mut_doc_new(nullptr);
    yyjson_mut_doc* doc_mut = (yyjson_mut_doc*)doc;
    yyjson_mut_val* root = yyjson_mut_arr(doc_mut);
    yyjson_mut_doc_set_root(doc_mut, root);

    for (size_t i = 0; i < n; ++i) {
        auto* obj = yyjson_mut_obj(doc_mut);
        yyjson_mut_obj_add_uint (doc_mut, obj, "id",    ids[i]);
        yyjson_mut_obj_add_bool (doc_mut, obj, "flag",  (ids[i] % 3U) == 0U);
        yyjson_mut_obj_add_uint (doc_mut, obj, "value", values[i]);
        yyjson_mut_obj_add_strcpy(doc_mut, obj, "name", names[i].c_str());
        yyjson_mut_arr_append(root, obj);
    }

    auto* json = yyjson_mut_write(doc_mut, YYJSON_WRITE_NOFLAG, nullptr);
    std::string result(json);
    free(json);
    yyjson_doc_free(doc);
    return result;
}