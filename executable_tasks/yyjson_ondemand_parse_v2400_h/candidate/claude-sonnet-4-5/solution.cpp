#include "interface.h"

#include <yyjson.h>

uint64_t compute_checksum(const std::string& json_input) {
  yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
  if (!doc) return 0;
  
  uint64_t sum = 0;
  yyjson_val* root = yyjson_doc_get_root(doc);
  
  size_t idx, max;
  yyjson_val* item;
  yyjson_arr_foreach(root, idx, max, item) {
    yyjson_val* id_val = yyjson_obj_get(item, "id");
    if (id_val) {
      sum += yyjson_get_uint(id_val);
    }
    
    yyjson_val* nested = yyjson_obj_get(item, "nested");
    if (nested) {
      yyjson_val* a_val = yyjson_obj_get(nested, "a");
      if (a_val) {
        sum += yyjson_get_uint(a_val);
      }
      
      yyjson_val* b_val = yyjson_obj_get(nested, "b");
      if (b_val) {
        sum += yyjson_get_uint(b_val);
      }
    }
  }
  
  yyjson_doc_free(doc);
  return sum;
}