#include "interface.h"
#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;

    yyjson_ondemand_parser parser;
    yyjson_ondemand_doc doc;
    yyjson_ondemand_parser_init(&parser, json_input.c_str(), json_input.size(), 0);
    yyjson_ondemand_doc_init(&doc);

    bool ok = yyjson_ondemand_parser_doc(&parser, json_input.c_str(), json_input.size(), &doc);
    if (!ok) return 0;

    yyjson_ondemand_val root;
    yyjson_ondemand_doc_get_root(&doc, &root);
    if (!yyjson_ondemand_val_is_arr(&root)) return 0;

    yyjson_ondemand_arr arr;
    yyjson_ondemand_val_to_arr(&root, &arr);

    yyjson_ondemand_val val;
    while (yyjson_ondemand_arr_next(&arr, &val)) {
        yyjson_ondemand_obj obj;
        yyjson_ondemand_val_to_obj(&val, &obj);

        yyjson_ondemand_val id_val;
        yyjson_ondemand_obj_get(&obj, "id", &id_val);
        sum += yyjson_val_get_uint64(yyjson_ondemand_val_get_value(&id_val));

        yyjson_ondemand_val nested_val;
        yyjson_ondemand_obj_get(&obj, "nested", &nested_val);
        yyjson_ondemand_obj nested_obj;
        yyjson_ondemand_val_to_obj(&nested_val, &nested_obj);

        yyjson_ondemand_val a_val, b_val;
        yyjson_ondemand_obj_get(&nested_obj, "a", &a_val);
        yyjson_ondemand_obj_get(&nested_obj, "b", &b_val);
        sum += yyjson_val_get_uint64(yyjson_ondemand_val_get_value(&a_val));
        sum += yyjson_val_get_uint64(yyjson_ondemand_val_get_value(&b_val));
    }

    yyjson_ondemand_doc_free(&doc);
    return sum;
}