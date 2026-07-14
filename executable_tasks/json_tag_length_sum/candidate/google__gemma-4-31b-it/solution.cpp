#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string json_padded(json_input);
    
    try {
        simdjson::ondemand::document doc = parser.iterate(json_padded);
        uint64_t sum = 0;
        
        simdjson::ondemand::array arr = doc.get_array();
        for (auto item : arr) {
            simdjson::ondemand::object obj = item.get_object();
            
            // We need to check 'flag' first. 
            // Note: simdjson fields are consumed as they are accessed.
            // To access multiple fields in any order, we can use the object's find/get methods.
            
            bool flag = false;
            uint64_t id = 0;
            uint64_t value = 0;
            size_t tag_len = 0;

            // Since the order of fields in JSON is not guaranteed, 
            // we iterate through the object or use specific lookups.
            // For performance in small objects, we can iterate once.
            for (auto field : obj) {
                std::string_view key = field.unescaped_key();
                if (key == "flag") {
                    flag = field.value_bool();
                } else if (key == "tag") {
                    tag_len = field.value_string().length();
                } else if (key == "id") {
                    id = field.value_uint64();
                } else if (key == "value") {
                    value = field.value_uint64();
                }
            }

            if (flag) {
                sum += id + tag_len + value;
            }
        }
        return sum;
    } catch (...) {
        return 0;
    }
}