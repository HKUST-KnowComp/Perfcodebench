#include "interface.h"
#include <yyjson.h>
#include <string>

std::string build_json(const std::vector<uint32_t>& ids,
                       const std::vector<uint32_t>& values,
                       const std::vector<std::string>& names) {
    yyjson_alc alc;
    yyjson_alc_pool_init(&alc, nullptr, yyjson_alc_default_size());
    yyjson_doc* doc = yyjson_mut_doc_new(&alc);
    yyjson_mut_val* arr = yyjson_mut_arr(doc);

    yyjson_mut_val name_val;
    name_val.tag = YYJSON_TYPE_STR | YYJSON_SUBTYPE_NOESC;

    const size_t n = ids.size();
    for (size_t i = 0; i < n; ++i) {
        uint32_t id = ids[i];
        bool flag   = (id % 3U) == 0U;
        uint32_t val = values[i];
        const std::string& nm = names[i];

        yyjson_mut_val* obj = yyjson_mut_obj(doc);
        yyjson_mut_obj_add(obj,
            yyjson_mut_raw(doc, "\"id\":", 5),
            yyjson_mut_uint(doc, id)
        );
        yyjson_mut_obj_add(obj,
            yyjson_mut_raw(doc, "\"flag\":", 7),
            yyjson_mut_bool(doc, flag)
        );
        yyjson_mut_obj_add(obj,
            yyjson_mut_raw(doc, "\"value\":", 8),
            yyjson_mut_uint(doc, val)
        );
        name_val.tag &= ~YYJSON_TYPE_MASK;
        name_val.tag |= YYJSON_TYPE_STR;
        name_val.uni.str = nm.c_str();
        yyjson_mut_obj_add(obj,
            yyjson_mut_raw(doc, "\"name\":", 7),
            &name_val
        );
        yyjson_mut_arr_append(arr, obj);
    }

    yyjson_doc_set_root(doc, arr);
    auto* res = yyjson_mut_write_opts(doc, YYJSON_WRITE_NOFLAG, &alc, nullptr, nullptr);
    std::string out(res);
    yyjson_mut_doc_free(doc);
    yyjson_alc_pool_release(&alc);
    return out;
}