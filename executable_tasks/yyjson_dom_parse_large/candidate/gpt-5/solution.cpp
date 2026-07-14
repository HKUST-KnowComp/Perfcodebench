#include "interface.h"
#include "yyjson.h"

#include <cstddef>

uint64_t compute_checksum(const std::string& json_input) {
  uint64_t sum = 0;
  if (json_input.empty()) return sum;

  yyjson_doc* doc = yyjson_read(json_input.data(), json_input.size(), YYJSON_READ_NOFLAG);
  if (!doc) return sum;

  yyjson_val* root = yyjson_doc_get_root(doc);
  if (!root || yyjson_get_type(root) != YYJSON_TYPE_ARR) {
    yyjson_doc_free(doc);
    return sum;
  }

  yyjson_arr_iter arr_it;
  yyjson_arr_iter_init(root, &arr_it);

  const char k_id[] = "id";
  const char k_x[] = "x";
  const char k_vals[] = "vals";

  yyjson_val* obj;
  while ((obj = yyjson_arr_iter_next(&arr_it))) {
    yyjson_val* idv = yyjson_obj_getn(obj, k_id, sizeof(k_id) - 1);
    if (idv) sum += yyjson_get_uint(idv);

    yyjson_val* xv = yyjson_obj_getn(obj, k_x, sizeof(k_x) - 1);
    if (xv) sum += yyjson_get_uint(xv);

    yyjson_val* valsv = yyjson_obj_getn(obj, k_vals, sizeof(k_vals) - 1);
    if (valsv && yyjson_get_type(valsv) == YYJSON_TYPE_ARR) {
      yyjson_val* sec = yyjson_arr_get(valsv, 1);
      if (sec) sum += yyjson_get_uint(sec);
    }
  }

  yyjson_doc_free(doc);
  return sum;
}
